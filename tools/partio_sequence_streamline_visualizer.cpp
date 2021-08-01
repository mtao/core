#include <mtao/opengl/Window.h>
#include <numeric>
#include <igl/AABB.h>
#include <mtao/geometry/mesh/read_obj.hpp>
#include <mtao/opengl/objects/mesh.h>
#include <mtao/opengl/objects/plane.hpp>
#include <tbb/parallel_for.h>
#include <mtao/opengl/drawables.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Corrade/Utility/Arguments.h>
#include <iostream>
#include <fstream>
#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <filesystem>
#include <variant>
#include <set>
#include <mtao/types.hpp>
#include <mtao/eigen/slice.hpp>


//struct Filter {
//    std::string name;
//    // specifies if the constraint wants values before or after a value
//    enum Type : char { Above,
//                       Below };
//    Type type;
//    double value;
//    // active index or not
//    std::optional<int> index;
//};
struct Particles {
    mtao::ColVecs3d positions;
    mtao::ColVecs3d velocities;
    //std::map<std::string, mtao::VecXd> values;

    mtao::VecXd density;
    Particles() = default;
    Particles(Particles &&) = default;
    Particles &operator=(Particles &&) = default;
    Particles(const std::filesystem::path &filepath);
    mtao::ColVecs3d positions_from_indices(const std::vector<int> &P) const;
    mtao::ColVecs3d velocities_from_indices(const std::vector<int> &P) const;
    mtao::VecXd densities_from_indices(const std::vector<int> &P) const;

    int count() const {
        return positions.cols();
    }
};

struct Filter {
    using BoolVec = Eigen::Array<bool, Eigen::Dynamic, 1>;

    virtual BoolVec particle_mask(const Particles &p) const = 0;
    std::vector<int> selected_particles(const Particles &p) const;
    virtual bool gui() { return false; }
};

std::vector<int> Filter::selected_particles(const Particles &p) const {

    auto query = particle_mask(p);
    std::vector<int> indices;
    for (auto &&[idx, val] : mtao::iterator::enumerate(query)) {
        if (val) {
            indices.emplace_back(idx);
        }
    }
    return indices;
}

struct PlaneFilter : public Filter
  , public mtao::opengl::objects::Plane {

    BoolVec particle_mask(const Particles &p) const override {

        const auto &V = p.positions;
        auto P = get_equation();

        auto vals = (P.transpose() * V.cast<float>().colwise().homogeneous()).eval();
        auto query = (vals.cwiseAbs().array() < 1.f).eval();
        return query.transpose();
    }
    bool gui() override {
        return mtao::opengl::objects::Plane::gui();
    }
};

struct MeshFilter : public Filter {
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::AABB<Eigen::MatrixXf, 3> aabb;
    float mesh_distance = .1;
    MeshFilter(const Eigen::MatrixXf &V, const Eigen::MatrixXi &F) : V(std::move(V)), F(std::move(F)) {

        aabb.init(V, F);
    }
    BoolVec particle_mask(const Particles &p) const override {
        Eigen::MatrixXf C;
        mtao::VecXf sqrD;
        mtao::VecXi I;


        auto P = p.positions.transpose().cast<float>().eval();
        aabb.squared_distance(V, F, P, sqrD, I, C);
        spdlog::info("Got squared dist");
        return sqrD.array() < mesh_distance * mesh_distance;
    }
    bool gui() override {
        bool changed = false;
        changed |= ImGui::InputFloat("Distance", &mesh_distance);
        return changed;
    }
};

struct IntersectionFilter : public Filter {
    std::vector<std::shared_ptr<Filter>> filters;
    BoolVec particle_mask(const Particles &p) const override {
        auto it = filters.begin();
        BoolVec R = (*it)->particle_mask(p);
        ++it;
        for (; it != filters.end(); ++it) {
            R = R && (*it)->particle_mask(p);
        }
        return R;
    }
    bool gui() override {
        return false;
    }
};

Particles::Particles(const std::filesystem::path &filepath) {
    mtao::geometry::point_cloud::PartioFileReader pfr(filepath);
    density = pfr.densities();
    positions = pfr.positions();
    velocities = pfr.velocities();
}

mtao::ColVecs3d Particles::velocities_from_indices(const std::vector<int> &P) const {
    return mtao::eigen::col_slice(velocities, P);
}
mtao::ColVecs3d Particles::positions_from_indices(const std::vector<int> &P) const {
    return mtao::eigen::col_slice(positions, P);
}

mtao::VecXd Particles::densities_from_indices(const std::vector<int> &P) const {
    return mtao::eigen::row_slice(density, P);
}
class MeshViewer : public mtao::opengl::Window3 {
    std::filesystem::path base_dir;
    std::string relative_format = "frame_{}/particles.bgeo.gz";
    std::vector<Particles> particles;
    enum ParticleColorMode : char { All,
                                    Selected,
                                    Density };
    ParticleColorMode particle_color_mode = ParticleColorMode::All;


    std::vector<int> active_indices;
    int current_frame = -1;

    Magnum::Shaders::Flat3D _flat_shader;
    Magnum::Shaders::VertexColor3D _vcolor_shader;
    Magnum::Shaders::Phong _phong_shader;

    mtao::opengl::objects::Mesh<3> point_mesh;
    //mtao::opengl::MeshDrawable<Magnum::Shaders::Flat3D> *point_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor3D> *point_drawable = nullptr;

    mtao::opengl::objects::Mesh<3> mesh;
    //mtao::opengl::MeshDrawable<Magnum::Shaders::Flat3D> *point_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Phong> *mesh_drawable = nullptr;


    std::shared_ptr<PlaneFilter> plane_filter;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Phong> *_plane_viewer = nullptr;
    std::shared_ptr<MeshFilter> mesh_filter;
    std::shared_ptr<IntersectionFilter> intersection_filter;

  public:
    MeshViewer(const Arguments &args);
    void gui() override;

    const Particles &frame_particles(int index) const;
    mtao::ColVecs3d frame_positions(int index) const;
    mtao::ColVecs3d frame_velocities(int index) const;
    mtao::VecXd frame_densities(int index) const;
    bool frame_exists(int index) const;
    int frame_file_count() const;

    void draw() override;

  private:
    void initialize_drawables();
    void current_frame_updated();
    void set_frame(int index);

    void reset_all_indices();
    void select_particles(std::vector<int> &&indices, bool set_active);

    void select_particles_from_plane(bool set_active);
    void select_particles_from_mesh(bool set_active);

    void select_particles_from_all(bool set_active);

    std::string frame_fmt() const;
    std::filesystem::path frame_path(int index) const;
    Particles frame_particles_from_disk(int index) const;

    std::filesystem::path mesh_path() const;
};

std::filesystem::path MeshViewer::mesh_path() const {
    return base_dir / "mesh.obj";
}

std::string MeshViewer::frame_fmt() const {
    return base_dir / relative_format;
}
std::filesystem::path MeshViewer::frame_path(int index) const {
    return fmt::format(frame_fmt(), index);
}
Particles MeshViewer::frame_particles_from_disk(int index) const {
    return Particles(frame_path(index));
}

const Particles &MeshViewer::frame_particles(int index) const {
    return particles.at(index);
}
mtao::ColVecs3d MeshViewer::frame_positions(int index) const {
    return frame_particles(index).positions_from_indices(active_indices);
}
mtao::ColVecs3d MeshViewer::frame_velocities(int index) const {
    return frame_particles(index).velocities_from_indices(active_indices);
}
mtao::VecXd MeshViewer::frame_densities(int index) const {
    return frame_particles(index).densities_from_indices(active_indices);
}

bool MeshViewer::frame_exists(int index) const {
    auto path = frame_path(index);
    bool res = std::filesystem::exists(path);
    return res;
}
int MeshViewer::frame_file_count() const {
    int j;
    for (j = 0; frame_exists(j); ++j)
        ;
    return j;
}

MeshViewer::MeshViewer(const Arguments &args) : Window3(args) {
    //MeshViewer(const Arguments& args): Window3(args,Magnum::GL::Version::GL210), _wireframe_shader{} {
    Corrade::Utility::Arguments myargs;
    myargs.addArgument("path").parse(args.argc, args.argv);
    base_dir = myargs.value("path");

    spdlog::info("{} frames exist with format {}", frame_file_count(), frame_fmt());
    int num_frames = frame_file_count();
    particles.resize(num_frames);

    tbb::parallel_for(0, num_frames, [&](int j) {
        particles[j] = frame_particles_from_disk(j);
    });

    reset_all_indices();
    initialize_drawables();
    set_frame(-1);
}
void MeshViewer::gui() {
    if (point_drawable) {
        point_drawable->gui("Points");
    }
    int old_index = current_frame;
    if (ImGui::InputInt("Current frame", &current_frame)) {
        current_frame = std::clamp<int>(current_frame, 0, particles.size() - 1);
        if (current_frame != old_index) {
            current_frame_updated();
        }
    }

    if (plane_filter) {
        if (ImGui::TreeNode("Plane Filter")) {
            if (plane_filter->gui()) {
                select_particles_from_plane(false);
            }

            if (ImGui::Button("Select Particles")) {
                select_particles_from_plane(true);
            }

            ImGui::TreePop();
        }
    }
    if (mesh_filter) {
        if (ImGui::TreeNode("Mesh Filter")) {
            if (mesh_filter->gui()) {
                select_particles_from_mesh(false);
            }

            if (ImGui::Button("Select Particles")) {
                select_particles_from_mesh(true);
            }

            ImGui::TreePop();
        }
    }
    if (intersection_filter) {
        if (ImGui::TreeNode("All Filter")) {
            if (intersection_filter->gui()) {
                select_particles_from_all(false);
            }

            if (ImGui::Button("Select Particles")) {
                select_particles_from_all(true);
            }

            ImGui::TreePop();
        }
    }
    if (ImGui::Button("Clear Particle Selection")) {
        reset_all_indices();
        current_frame_updated();
    }
}

void MeshViewer::reset_all_indices() {
    active_indices.resize(particles[0].count());
    std::iota(active_indices.begin(), active_indices.end(), 0);
}

void MeshViewer::select_particles_from_mesh(bool set_active) {
    auto indices = mesh_filter->selected_particles(frame_particles(current_frame));
    select_particles(std::move(indices), set_active);
}


void MeshViewer::select_particles_from_plane(bool set_active) {
    auto indices = plane_filter->selected_particles(frame_particles(current_frame));
    select_particles(std::move(indices), set_active);
}

void MeshViewer::select_particles_from_all(bool set_active) {
    auto indices = intersection_filter->selected_particles(frame_particles(current_frame));
    select_particles(std::move(indices), set_active);
}

void MeshViewer::select_particles(std::vector<int> &&indices, bool set_active) {

    if (set_active) {

        active_indices = std::move(indices);
        particle_color_mode = ParticleColorMode::All;
        current_frame_updated();
    } else {

        reset_all_indices();
        particle_color_mode = ParticleColorMode::Selected;
        current_frame_updated();
    }
}

void MeshViewer::draw() {
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
    Window3::draw();
}

void MeshViewer::current_frame_updated() {
    set_frame(current_frame);
}
void MeshViewer::set_frame(int index) {
    spdlog::info("Setting frame {}", index);
    if (index < 0 || active_indices.empty()) {
        spdlog::info("no points selected, not drawing points");
        point_drawable->set_visibility(false);
    } else {
        point_drawable->set_visibility(true);
        auto current_positions = frame_positions(index).cast<float>().eval();
        point_mesh.setVertexBuffer(current_positions);
        mtao::ColVecs4f C(4, current_positions.cols());
        C.row(3).setConstant(1);
        auto RGB = C.topRows<3>();
        switch (particle_color_mode) {
        case ParticleColorMode::Selected: {
            RGB.setZero();
            for (auto &&idx : active_indices) {
                RGB.col(idx).setConstant(1);
            }
            break;
        }
        case ParticleColorMode::Density: {
            RGB.rowwise() = frame_densities(index).transpose().cast<float>();
            break;
        }
        default:
        case ParticleColorMode::All: {
            RGB.setOnes();
            break;
        }
        }
        point_mesh.setColorBuffer(C);
    }
}

void MeshViewer::initialize_drawables() {

    point_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor3D>{ point_mesh, _vcolor_shader, drawables() };
    point_drawable->point_size = 3.0;
    point_drawable->deactivate();
    point_drawable->activate_points();
    point_mesh.setParent(&root());

    {
        auto p = mesh_path();
        if (std::filesystem::exists(p)) {
            auto [V, F] = mtao::geometry::mesh::read_objF(p);
            V.row(1) *= -1;
            V.row(2) *= -1;
            mesh_filter = std::make_shared<MeshFilter>(V.transpose(), F.transpose());

            spdlog::info("Loading a mesh with {} vertices and {} faces", V.cols(), F.cols());
            mesh.setTriangleBuffer(V, F.cast<unsigned int>());

            mesh_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Phong>{ mesh, _phong_shader, drawables() };
            mesh.setParent(&root());
        } else {
            spdlog::info("Mesh file {} not found", std::string(p));
        }
    }

    plane_filter = std::make_shared<PlaneFilter>();

    plane_filter->setParent(&root());
    _plane_viewer = new mtao::opengl::MeshDrawable<Magnum::Shaders::Phong>{ *plane_filter, _phong_shader, drawables() };
    _plane_viewer->data().diffuse_color = { .32, .32, .32, 1. };
    _plane_viewer->data().ambient_color = { .32, .32, .32, 1. };

    intersection_filter = std::make_shared<IntersectionFilter>();
    intersection_filter->filters.emplace_back(plane_filter);
    intersection_filter->filters.emplace_back(mesh_filter);


    //_plane_viewer->deactivate();
    //point_drawable->deactivate();
    //mesh_drawable->deactivate();
}

MAGNUM_APPLICATION_MAIN(MeshViewer)
