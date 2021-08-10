#include "viewer.hpp"
#include <mtao/iterator/enumerate.hpp>
#include <spdlog/spdlog.h>
#include <iostream>
#include <numeric>
#include <fstream>
#include <fmt/format.h>
#include <tbb/parallel_for.h>
#include <mtao/geometry/mesh/read_obj.hpp>
#include <mtao/geometry/mesh/shapes/tube.hpp>

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
    const auto &p = frame_particles(index);
    if (show_all_particles) {
        return p.positions;
    } else {
        return p.positions_from_indices(active_indices);
    }
}
mtao::ColVecs3d MeshViewer::frame_velocities(int index) const {
    const auto &p = frame_particles(index);
    if (show_all_particles) {
        return p.velocities;
    } else {
        return p.velocities_from_indices(active_indices);
    }
}
mtao::VecXd MeshViewer::frame_densities(int index) const {
    const auto &p = frame_particles(index);
    if (show_all_particles) {
        return p.densities;
    } else {
        return p.densities_from_indices(active_indices);
    }
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
        ImGui::InputFloat("Point size", &point_drawable->point_size);
    }
    if (tail_size) {
        if (ImGui::InputInt("Tail size", &*tail_size)) {
            tail_size = std::max<int>(2, *tail_size);
            current_frame_updated();
        }
        if (ImGui::Checkbox("Tube Geometry", &pipe_geometry)) {
            current_frame_updated();
        }
        if (ImGui::InputInt("Tube subdivision", &tube_subdivisions)) {
            tube_subdivisions = std::max<int>(3, tube_subdivisions);
            current_frame_updated();
        }
        if (ImGui::InputFloat("Tube radius", &tube_radius)) {
            current_frame_updated();
        }
        if (ImGui::Button("Show Points")) {
            tail_size = {};
        }
    } else {
        if (ImGui::Button("Use Tail")) {
            tail_size = 2;
            current_frame_updated();
        }
    }

    {
        static const char *items[] = {
            "All",
            "Selected",
            "Velocity",
            "Density",
        };
        int m = static_cast<char>(particle_color_mode);
        if (ImGui::Combo("Type", &m, items, IM_ARRAYSIZE(items))) {
            particle_color_mode = static_cast<ParticleColorMode>(char(m));
            current_frame_updated();
        }
    }
    if (ImGui::TreeNode("Colormap settings")) {
        if (colmap_widget.gui()) {
            current_frame_updated();
        }
        ImGui::TreePop();
    }
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
    if (sphere_filter) {
        if (ImGui::TreeNode("Sphere Filter")) {
            if (sphere_filter->gui()) {
                select_particles_from_sphere(false);
            }

            if (ImGui::Button("Select Particles")) {
                select_particles_from_sphere(true);
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
    if (prune_filter) {
        if (ImGui::TreeNode("Prune Filter")) {
            if (prune_filter->gui()) {
                select_particles_from_prune(false);
            }

            if (ImGui::Button("Select Particles")) {
                select_particles_from_prune(true);
            }

            ImGui::TreePop();
        }
    }
    if (range_filter) {
        if (ImGui::TreeNode("Range Filter")) {
            if (range_filter->gui()) {
                select_particles_from_range(false);
            }

            if (ImGui::Button("Select Particles")) {
                select_particles_from_range(true);
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

    if (mesh_drawable) {
        mesh_drawable->gui("Mesh Viewer");
    }
    if (_plane_viewer) {
        _plane_viewer->gui("Plane Viewer");
    }
    if (_sphere_viewer) {
        _sphere_viewer->gui("Sphere Viewer");
    }
}

void MeshViewer::reset_all_indices() {
    active_indices.resize(particles[0].count());
    std::iota(active_indices.begin(), active_indices.end(), 0);
    show_all_particles = true;
}

void MeshViewer::select_particles_from_mesh(bool set_active) {
    if (current_frame < 0) {
        return;
    }
    auto indices = mesh_filter->selected_particles(frame_particles(current_frame));
    select_particles(std::move(indices), set_active);
}

void MeshViewer::select_particles_from_sphere(bool set_active) {
    if (current_frame < 0) {
        return;
    }
    auto indices = sphere_filter->selected_particles(frame_particles(current_frame));
    select_particles(std::move(indices), set_active);
}

void MeshViewer::select_particles_from_plane(bool set_active) {
    if (current_frame < 0) {
        return;
    }
    auto indices = plane_filter->selected_particles(frame_particles(current_frame));
    select_particles(std::move(indices), set_active);
}
void MeshViewer::select_particles_from_prune(bool set_active) {
    if (current_frame < 0) {
        return;
    }
    auto indices = prune_filter->selected_particles(frame_particles(current_frame));
    select_particles(std::move(indices), set_active);
}
void MeshViewer::select_particles_from_range(bool set_active) {
    if (current_frame < 0) {
        return;
    }
    auto indices = range_filter->selected_particles(frame_particles(current_frame));
    select_particles(std::move(indices), set_active);
}

void MeshViewer::select_particles_from_all(bool set_active) {
    if (current_frame < 0) {
        return;
    }
    auto indices = intersection_filter->selected_particles(frame_particles(current_frame));
    select_particles(std::move(indices), set_active);
}

void MeshViewer::select_particles(std::vector<int> &&indices, bool set_active) {

    tail_size = {};
    show_all_particles = !set_active;
    active_indices = std::move(indices);
    if (set_active) {

        particle_color_mode = ParticleColorMode::All;
        current_frame_updated();
    } else {

        particle_color_mode = ParticleColorMode::Selected;
        current_frame_updated();
    }
}
std::tuple<mtao::ColVecs3d, mtao::VecXd> MeshViewer::get_pos_scalar(int index) const {
    std::tuple<mtao::ColVecs3d, mtao::VecXd> ret;
    auto &[V, F] = ret;
    V = frame_positions(index);
    F.resize(V.cols());
    switch (particle_color_mode) {
    case ParticleColorMode::Selected: {

        F.setZero();
        if (show_all_particles) {

            for (int idx : active_indices) {
                F(idx) = 1;
            }
        } else {

            F.setOnes();
        }

        break;
    }
    case ParticleColorMode::Density: {
        F = frame_densities(index);
        break;
    }
    case ParticleColorMode::Velocity: {
        F = frame_velocities(index).colwise().norm().transpose();
        break;
    }
    case ParticleColorMode::All:
    default: {
        F.setConstant(1);
        break;
    }
    }
    return ret;
}

std::vector<std::tuple<mtao::ColVecs3d, mtao::VecXd>> MeshViewer::get_pos_scalar_tail(int index) const {

    int last_index = index + 2;
    if (this->tail_size) {
        last_index = *this->tail_size + index;
    }
    last_index = std::min<int>(particles.size(), last_index);
    int tail_size = last_index - index;
    auto [VF, FF] = get_pos_scalar(index);


    std::vector<std::tuple<mtao::ColVecs3d, mtao::VecXd>> ret(VF.cols());
    for (auto &&[idx, pr] : mtao::iterator::enumerate(ret)) {
        auto &[V, F] = pr;
        V.resize(3, tail_size);
        V.col(0) = VF.col(idx);
        F.resize(tail_size);
        F(0) = FF(idx);
    }
    for (int j = 1; j < tail_size; ++j) {
        auto [VF, FF] = get_pos_scalar(index + j);
        for (auto &&[idx, pr] : mtao::iterator::enumerate(ret)) {
            auto &[V, F] = pr;
            V.col(j) = VF.col(idx);
            F(j) = FF(idx);
        }
    }

    return ret;
}

std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs2i> MeshViewer::get_pos_scalar_lines(int index) const {
    auto tails = get_pos_scalar_tail(index);

    std::vector<mtao::ColVecs3d> tV(tails.size());
    std::vector<mtao::VecXd> tF(tails.size());
    std::vector<mtao::ColVecs2i> tE(tails.size());

    int offset = 0;
    for (auto &&[TV, TF, TE, pr] : mtao::iterator::zip(tV, tF, tE, tails)) {
        auto &[V, F] = pr;

        TV = V;
        TF = F;
        TE.resize(2, TV.cols() - 1);
        for (int j = 0; j < TE.cols(); ++j) {
            auto e = TE.col(j);
            int val = j + offset;
            e << val, val + 1;
        }

        offset += TV.cols();
    }
    std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs2i> ret;
    {
        std::get<0>(ret) = mtao::eigen::hstack_iter(tV.begin(), tV.end());
        std::get<1>(ret) = mtao::eigen::vstack_iter(tF.begin(), tF.end());
        std::get<2>(ret) = mtao::eigen::hstack_iter(tE.begin(), tE.end());
        const auto &[V, F, T] = ret;
        spdlog::info("vertices {} function {} edges {}", V.cols(), F.size(), T.cols());
    }
    return ret;
}

std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs3i> MeshViewer::get_pos_scalar_tubes(int index) const {

    auto tails = get_pos_scalar_tail(index);

    mtao::geometry::mesh::shapes::internal::TubeConstructor tc{ tube_radius, tube_subdivisions, true, true };
    std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs3i> ret;

    //std::vector<std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs3i>> single_tubes(Vs.size());
    std::vector<mtao::ColVecs3d> tV(tails.size());
    std::vector<mtao::VecXd> tF(tails.size());
    std::vector<mtao::ColVecs3i> tT(tails.size());

    int offset = 0;
    for (auto &&[TV, TF, TT, pr] : mtao::iterator::zip(tV, tF, tT, tails)) {
        auto &[V, F] = pr;

        //std::cout << "Input Vertices\n"
        //          << V << std::endl;
        TV = tc.create_vertices(V);
        TF = tc.create_per_ring_data_rows(F);
        TT = tc.create_triangulation(V.cols());
        if (TT.maxCoeff() > TV.cols()) {
            spdlog::info("Triangle indices are bad, too big ({} > {})", TT.maxCoeff(), TV.cols());
        }
        if (TT.minCoeff() < 0) {
            spdlog::info("Triangle small index found: {}", TT.minCoeff());
        }
        TT.array() += offset;

        offset += TV.cols();
    }
    {
        std::get<0>(ret) = mtao::eigen::hstack_iter(tV.begin(), tV.end());
        std::get<1>(ret) = mtao::eigen::vstack_iter(tF.begin(), tF.end());
        std::get<2>(ret) = mtao::eigen::hstack_iter(tT.begin(), tT.end());
        const auto &[V, F, T] = ret;
        spdlog::info("vertices {} function {} triangles {}", V.cols(), F.size(), T.cols());
        //std::cout << "V:\n"
        //          << V << std::endl;
        //std::cout << "F:\n"
        //          << F << std::endl;
        //std::cout << "T:\n"
        //          << T << std::endl;
    }
    return ret;
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
    if (index < 0) {
        spdlog::info("no points selected, not drawing points");
        point_drawable->set_visibility(false);
        tube_drawable->set_visibility(false);
    } else if (tail_size) {
        point_drawable->set_visibility(false);
        tube_drawable->set_visibility(true);

        if (pipe_geometry) {
            tube_drawable->deactivate();
            tube_drawable->activate_triangles();
            auto [V, F, T] = get_pos_scalar_tubes(index);

            tube_mesh.setTriangleBuffer(V.cast<float>().eval(), T.cast<unsigned int>().eval());
            mtao::ColVecs4f C = colmap_widget.get_rgba(F.cast<float>().eval());
            tube_mesh.setColorBuffer(C);
        } else {
            tube_drawable->deactivate();
            tube_drawable->activate_edges();
            auto [V, F, E] = get_pos_scalar_lines(index);

            tube_mesh.setEdgeBuffer(V.cast<float>().eval(), E.cast<unsigned int>().eval());
            mtao::ColVecs4f C = colmap_widget.get_rgba(F.cast<float>().eval());
            std::cout << std::endl;
            tube_mesh.setColorBuffer(C);
        }


    } else {
        point_drawable->set_visibility(true);
        tube_drawable->set_visibility(false);
        auto current_positions = frame_positions(index).cast<float>().eval();
        auto [V, F] = get_pos_scalar(index);
        point_mesh.setVertexBuffer(V.cast<float>().eval());
        mtao::ColVecs4f C = colmap_widget.get_rgba(F.cast<float>().eval());
        point_mesh.setColorBuffer(C);
    }
}

void MeshViewer::initialize_drawables() {

    point_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor3D>{ point_mesh, _vcolor_shader, drawables() };
    point_drawable->point_size = 3.0;
    point_drawable->deactivate();
    point_drawable->activate_points();
    point_mesh.setParent(&root());


    tube_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor3D>{ tube_mesh, _vcolor_shader, drawables() };
    tube_drawable->deactivate();
    tube_drawable->activate_triangles();
    tube_mesh.setParent(&root());
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

    sphere_filter = std::make_shared<SphereFilter>();

    sphere_filter->setParent(&root());
    _sphere_viewer = new mtao::opengl::MeshDrawable<Magnum::Shaders::Phong>{ *sphere_filter, _phong_shader, drawables() };
    _sphere_viewer->data().diffuse_color = { .32, .32, .32, 1. };
    _sphere_viewer->data().ambient_color = { .32, .32, .32, 1. };

    plane_filter = std::make_shared<PlaneFilter>();

    plane_filter->setParent(&root());
    _plane_viewer = new mtao::opengl::MeshDrawable<Magnum::Shaders::Phong>{ *plane_filter, _phong_shader, drawables() };
    _plane_viewer->data().diffuse_color = { .32, .32, .32, 1. };
    _plane_viewer->data().ambient_color = { .32, .32, .32, 1. };

    range_filter = std::make_shared<RangeFilter>();
    prune_filter = std::make_shared<PruneFilter>();

    intersection_filter = std::make_shared<IntersectionFilter>();
    intersection_filter->filters.emplace_back("Plane", plane_filter, false);
    intersection_filter->filters.emplace_back("Mesh", mesh_filter, false);
    intersection_filter->filters.emplace_back("Sphere", sphere_filter, false);
    intersection_filter->filters.emplace_back("Range", range_filter, false);
    intersection_filter->filters.emplace_back("Prune", prune_filter, false);


    //_plane_viewer->deactivate();
    //point_drawable->deactivate();
    //mesh_drawable->deactivate();
}
