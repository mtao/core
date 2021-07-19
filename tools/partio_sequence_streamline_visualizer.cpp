#include <mtao/opengl/Window.h>
#include <numeric>
#include <mtao/opengl/objects/mesh.h>
#include <mtao/opengl/drawables.h>
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


struct Filter {
    std::string name;
    // specifies if the constraint wants values before or after a value
    enum Type : char { Above,
                       Below };
    Type type;
    double value;
    // active index or not
    std::optional<int> index;
};
struct Particles {
    mtao::ColVecs3d positions;
    mtao::ColVecs3d velocities;
    //std::map<std::string, mtao::VecXd> values;

    mtao::VecXd density;
    Particles(const std::filesystem::path &filepath);
    mtao::ColVecs3d selected_indices(const std::vector<int> &P) const;

    int count() const {
        return positions.cols();
    }
};

Particles::Particles(const std::filesystem::path &filepath) {
    mtao::geometry::point_cloud::PartioFileReader pfr(filepath);
    density = pfr.densities();
    positions = pfr.positions();
    //velocities = pfr.velocities();
}
mtao::ColVecs3d Particles::selected_indices(const std::vector<int> &P) const {
    return mtao::eigen::col_slice(positions, P);
}
class MeshViewer : public mtao::opengl::Window3 {
    std::filesystem::path base_dir;
    std::string relative_format = "frame_{}/particles.bgeo.gz";
    std::vector<Particles> particles;


    std::vector<int> active_indices;
    int current_frame = -1;

    Magnum::Shaders::Flat3D _flat_shader;

    mtao::opengl::objects::Mesh<3> point_mesh;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat3D> *point_drawable = nullptr;

  public:
    MeshViewer(const Arguments &args);
    void gui() override;

    const Particles &frame_particles(int index) const;
    mtao::ColVecs3d frame_positions(int index) const;
    bool frame_exists(int index) const;
    int frame_file_count() const;

  private:
    void initialize_drawables();
    void current_frame_updated();
    void set_frame(int index);

    std::string frame_fmt() const;
    std::filesystem::path frame_path(int index) const;
    Particles frame_particles_from_disk(int index) const;
};

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
    return frame_particles(index).selected_indices(active_indices);
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
    particles.reserve(num_frames);
    for (int j = 0; j < num_frames; ++j) {
        particles.emplace_back(frame_particles_from_disk(j));
    }

    active_indices.resize(particles[0].count());
    std::iota(active_indices.begin(), active_indices.end(), 0);
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
        spdlog::info("Current positions");
        point_mesh.setVertexBuffer(current_positions);
    }
}

void MeshViewer::initialize_drawables() {

    point_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat3D>{ point_mesh, _flat_shader, drawables() };
    point_drawable->deactivate();
    point_drawable->activate_points();
    point_mesh.setParent(&root());
}

MAGNUM_APPLICATION_MAIN(MeshViewer)
