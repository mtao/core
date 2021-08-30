#include "viewer.hpp"
#include <mtao/iterator/enumerate.hpp>
#include <numbers>
#include <spdlog/spdlog.h>
#include <iostream>
#include <numeric>
#include <fstream>
#include <fmt/format.h>
#include <tbb/parallel_for.h>
#include <mtao/geometry/mesh/read_obj.hpp>
#include <nlohmann/json.hpp>

std::filesystem::path MeshViewer::mesh_path() const {
    std::filesystem::path local_path = base_dir / "mesh.obj";
    if (std::filesystem::exists(local_path)) {
        return local_path;
    }

    std::filesystem::path meshinfo_path = base_dir / "mesh_info.json";
    if (std::filesystem::exists(meshinfo_path)) {
        nlohmann::json js;
        std::ifstream(meshinfo_path) >> js;
        std::cout << js << std::endl;

        local_path = js["mesh"]["filename"].get<std::string>();
        std::cout << "Local path: " << local_path << std::endl;
        if (std::filesystem::exists(local_path)) {
            return local_path;
        }
    }
    spdlog::warn("No valid mesh file path found");
    return {};
}

std::string MeshViewer::frame_fmt() const {
    return base_dir / relative_format;
}
std::filesystem::path MeshViewer::frame_path(int index) const {
    return fmt::vformat(frame_fmt(), fmt::make_format_args(index));
}
Particles MeshViewer::frame_particles_from_disk(int index) const {
    return Particles(frame_path(index));
}

const Particles &MeshViewer::frame_particles(int index) const {
    return particles.at(index);
}
mtao::ColVecs3d MeshViewer::frame_positions(int index) const {
    return tube_mesh_gui.frame_positions(index);
}
mtao::ColVecs3d MeshViewer::frame_velocities(int index) const {
    return tube_mesh_gui.frame_velocities(index);
}
mtao::VecXd MeshViewer::frame_densities(int index) const {
    return tube_mesh_gui.frame_densities(index);
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

MeshViewer::MeshViewer(const Arguments &args) : Window3(args), tube_mesh_gui(particles, active_indices, show_all_particles, _colmap_widget, drawables()) {
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


    if (ImGui::TreeNode("Config")) {
        if (ImGui::Button("Load ")) {
            load_settings();
        }
        if (ImGui::Button("Save ")) {
            save_settings();
        }
        if (config_path.size() < 1000) {
            config_path.resize(1000);
        }
        if (ImGui::InputText("Path", &config_path[0], config_path.size())) {}
        if (std::filesystem::is_regular_file(config_path)) {
            ImGui::Text("File Exists");
        } else {
            ImGui::Text("File not Found");
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Colormap settings")) {
        if (_colmap_widget.gui()) {
            current_frame_updated();
        }
        ImGui::TreePop();
    }
    if (point_drawable) {
        ImGui::InputFloat("Point size", &point_drawable->point_size);
    }
    if (ImGui::Checkbox("Do Particle Filtering", &do_particle_filtering)) {
        if (do_particle_filtering) {
            if (show_tubes) {
                show_tubes = false;
                current_frame_updated();
            }
        }
    }
    int old_index = current_frame;
    if (ImGui::InputInt("Current frame", &current_frame)) {
        current_frame = std::clamp<int>(current_frame, 0, particles.size() - 1);
        if (current_frame != old_index) {
            current_frame_updated();
        }
    }
    filter_gui();


    if (do_particle_filtering) {
        particle_gui();
    } else {
        tube_gui();
    }
    drawable_gui();
}


void MeshViewer::tube_gui() {
    if (ImGui::Checkbox("Show Tubes", &show_tubes)) {
        current_frame_updated();
    }
    if (show_tubes) {
        if (ImGui::TreeNode("Tube Mesh")) {
            if (tube_mesh_gui.gui()) {
                current_frame_updated();
            }
            ImGui::TreePop();
        }
    }
    if (ImGui::Button("Save PLY")) {
        tube_mesh_gui.save_ply(current_frame, "tubes_{:04}.ply");
    }
}


void MeshViewer::particle_gui() {
    ImGui::Text("Particle gui");
}


void MeshViewer::filter_gui() {
    if (ImGui::TreeNode("Filters")) {
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
        ImGui::TreePop();
    }
}
void MeshViewer::drawable_gui() {
    if (point_drawable) {
        point_drawable->gui("Points");
    }
    if (mesh_drawable) {
        mesh_drawable->gui("Mesh Viewer");
        if (ImGui::InputFloat("Mesh Scale", &mesh_scale)) {
            update_mesh_orientation();
        }

        if (ImGui::InputFloat3("Mesh Rotation", euler_angles.data())) {
            update_mesh_orientation();
        }
    }
    if (ImGui::Button("Hide filter geometry")) {
        if (_plane_viewer) {
            _plane_viewer->set_visibility(false);
        }
        if (_sphere_viewer) {
            _sphere_viewer->set_visibility(false);
        }
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
    auto [start, end] = filter_interval(current_frame);
    auto indices = range_filter->selected_particles(particles, start, end);
    select_particles(std::move(indices), set_active);
}

void MeshViewer::select_particles_from_all(bool set_active) {
    if (current_frame < 0) {
        return;
    }
    auto indices = intersection_filter->selected_particles(frame_particles(current_frame));
    select_particles(std::move(indices), set_active);
}

void MeshViewer::select_particles(std::vector<int> &&indices, bool set_active, bool deactivate_tubes) {

    show_tubes &= deactivate_tubes;

    show_all_particles = !set_active;
    active_indices = std::move(indices);
    if (set_active) {

        tube_mesh_gui.set_scalar_function_mode(TubeMeshConstructor::ScalarFunctionMode::All);
        current_frame_updated();
    } else {
        tube_mesh_gui.set_scalar_function_mode(TubeMeshConstructor::ScalarFunctionMode::Selected);
        current_frame_updated();
    }
}

std::array<int, 2> MeshViewer::filter_interval(int index) const {

    return tube_mesh_gui.valid_interval(index);
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
        tube_mesh_gui.set_visibility(false);
    } else if (show_tubes) {
        point_drawable->set_visibility(false);
        tube_mesh_gui.set_visibility(true);

        spdlog::info("Updating tube geometry");

        tube_mesh_gui.update(index);

    } else {
        spdlog::info("Updating point data");
        point_drawable->set_visibility(true);
        tube_mesh_gui.set_visibility(false);
        auto current_positions = frame_positions(index).cast<float>().eval();
        auto [V, F] = tube_mesh_gui.get_pos_scalar(index);
        point_mesh.setVertexBuffer(V.cast<float>().eval());
        mtao::ColVecs4f C = tube_mesh_gui.colmap_widget().get_rgba(F.cast<float>().eval());
        point_mesh.setColorBuffer(C);
    }
}

void MeshViewer::initialize_drawables() {

    point_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor3D>{ point_mesh, _vcolor_shader, drawables() };
    point_drawable->point_size = 3.0;
    point_drawable->deactivate();
    point_drawable->activate_points();
    point_mesh.setParent(&root());


    tube_mesh_gui.deactivate();
    tube_mesh_gui.activate_triangles();
    tube_mesh_gui.setParent(&root());
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
    _sphere_viewer->set_visibility(false);

    plane_filter = std::make_shared<PlaneFilter>();

    plane_filter->setParent(&root());
    _plane_viewer = new mtao::opengl::MeshDrawable<Magnum::Shaders::Phong>{ *plane_filter, _phong_shader, drawables() };
    _plane_viewer->data().diffuse_color = { .32, .32, .32, 1. };
    _plane_viewer->data().ambient_color = { .32, .32, .32, 1. };
    _plane_viewer->set_visibility(false);

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
void MeshViewer::update_mesh_orientation() {
    // asdf
    mesh.resetTransformation();
    auto tr = [](double v) -> double {
        return std::numbers::pi_v<double> * v / 180.0;
    };
    auto tr_m = [](double v) -> Magnum::Deg {
        return Magnum::Deg(v);
    };

    mesh.rotate(tr_m(euler_angles[0]), Magnum::Vector3::xAxis());
    mesh.rotate(tr_m(euler_angles[1]), Magnum::Vector3::yAxis());
    mesh.rotate(tr_m(euler_angles[2]), Magnum::Vector3::zAxis());
    mesh.scale(Magnum::Vector3(mesh_scale));

    using namespace Eigen;
    if (mesh_filter) {
        mesh_filter->point_transform.setIdentity();
        mesh_filter->point_transform.scale(mesh_scale);

        mesh_filter->point_transform.rotate(AngleAxisd(tr(euler_angles[0]), Vector3d::UnitX()));
        mesh_filter->point_transform.rotate(AngleAxisd(tr(euler_angles[1]), Vector3d::UnitY()));
        mesh_filter->point_transform.rotate(AngleAxisd(tr(euler_angles[2]), Vector3d::UnitZ()));
        mesh_filter->point_transform =
          mesh_filter->point_transform.inverse();
    }
}


void MeshViewer::save_settings(const std::filesystem::path &path) const {

    nlohmann::json js;
    js["tube_mesh_gui"] = tube_mesh_gui.config();
    auto &filters = js["filters"];

    filters["sphere"] = sphere_filter->config();
    filters["plane"] = plane_filter->config();
    filters["mesh"] = mesh_filter->config();
    filters["intersection"] = intersection_filter->config();
    filters["range"] = range_filter->config();
    filters["prune"] = prune_filter->config();

    {// read off colormap settings
        auto &colmap_js = js["colormap"];
        colmap_js["type"] =
          mtao::visualization::imgui::ColorMapSettingsWidget::ColorMapNames[int(_colmap_widget.get_type())];
        {
            colmap_js["scale"] = _colmap_widget.scale;
            colmap_js["shift"] = _colmap_widget.shift;
        }
    }


    std::ofstream ofs(path);
    ofs << js;
}
void MeshViewer::load_settings(const std::filesystem::path &path) {

    std::ifstream ifs(path);
    nlohmann::json js;
    ifs >> js;
    tube_mesh_gui.load_config(js["tube_mesh_gui"]);
    auto &filters = js["filters"];

    sphere_filter->load_config(filters["sphere"]);
    plane_filter->load_config(filters["plane"]);
    mesh_filter->load_config(filters["mesh"]);
    intersection_filter->load_config(filters["intersection"]);
    range_filter->load_config(filters["range"]);
    prune_filter->load_config(filters["prune"]);

    {// read off colormap settings

        auto colmap_js = [&]() -> nlohmann::json {
            if (js.contains("colormap")) {
                return js["colormap"];
            } else if (js.contains("tube_mesh_gui")) {
                const auto &tmgjs = js["tube_mesh_gui"];
                if (tmgjs.contains("colormap")) {
                    {
                        return tmgjs["colormap"];
                    }
                }
            }
            return {};
        }();
        const std::string colmap_type = colmap_js["type"].get<std::string>();
        for (auto &&[index, name] : mtao::iterator::enumerate(mtao::visualization::imgui::ColorMapSettingsWidget::ColorMapNames)) {
            if (name == colmap_type) {
                _colmap_widget.set_type(
                  mtao::visualization::imgui::ColorMapSettingsWidget::ColorMapType(index));
            }
        }
        {
            if (colmap_js.contains("min")) {
                _colmap_widget.min_value = colmap_js["min"].get<int>();
                _colmap_widget.max_value = colmap_js["max"].get<int>();
                _colmap_widget.update_scale_shift();
            } else {
                _colmap_widget.scale = colmap_js["scale"].get<int>();
                _colmap_widget.shift = colmap_js["shift"].get<int>();
                _colmap_widget.update_minmax();
            }
        }
    }
}
void MeshViewer::save_settings() const {
    save_settings(config_path);
}
void MeshViewer::load_settings() {
    load_settings(config_path);
}

void MeshViewer::save_filtered_particles(const std::string &path_fmt) {

    if (show_all_particles) {
        spdlog::error("Cannot save filtered particles while showing all of them. Odds are you're wasting space by copying the input data");
        return;
    }
    auto parent_path = std::filesystem::path(path_fmt).parent_path();
    if (std::filesystem::exists(parent_path)) {
        if (!std::filesystem::is_directory(parent_path)) {
            spdlog::error("Cannot save filtered particles because [{}] is not a directory", std::string(parent_path));
        }
    } else {
        bool created_parent = std::filesystem::create_directories(parent_path);
        if (created_parent) {
            spdlog::info("Created directory to save particles {}", std::string(parent_path));
        }
    }

    bool do_write = overwrite_old_particles;
    // check if any file we would write will be overwritten
    if (!overwrite_old_particles) {
        for (size_t index = 0; index < particles.size(); ++index) {

            std::filesystem::path path = fmt::vformat(path_fmt, fmt::make_format_args(index));
            do_write = overwrite_old_particles || !std::filesystem::exists(path);
            if (do_write) {

                spdlog::error("Cannot write particles because a file already exists, Check the directory or enable overwriting");
                break;
            }
        }
    }
    {
        // check if there's a file that might accidnetally result in a continuation of hte current sequence
        std::filesystem::path path = fmt::vformat(path_fmt, fmt::make_format_args(particles.size()));
        if (std::filesystem::exists(path)) {
            if (overwrite_old_particles) {

                spdlog::warn("Cannot write particles because a continuation could happen creating an invalid sequence");
            } else {
                spdlog::error("Cannot write particles because a continuation could happen creating an invalid sequence. This can be forced with overwritting enabled");
                do_write = false;
            }
        }
    }

    if (do_write) {
        for (auto &&[index, parts] : mtao::iterator::enumerate(particles)) {

            std::filesystem::path path = fmt::vformat(path_fmt, fmt::make_format_args(index));

            parts.save_subset(path, active_indices);
        }
    }
}
void MeshViewer::save_filtered_particles() {
    save_filtered_particles(particles_output_path);
}
