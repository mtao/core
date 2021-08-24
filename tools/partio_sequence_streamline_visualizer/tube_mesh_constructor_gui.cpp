
#include <mtao/geometry/mesh/write_ply.hpp>
#include <mtao/iterator/enumerate.hpp>
#include <imgui.h>
#include "tube_mesh_constructor_gui.hpp"
#include <mtao/visualization/imgui/utils.hpp>
#include <fstream>
const std::array<std::string, int(TubeMeshConstructor::ScalarFunctionMode::END)> TubeMeshConstructor::ScalarFunctionModeNames = {
    "All",
    "Selected",
    "Velocity",
    "Density",
};
void TubeMeshConstructorGui::update(int index) {
    if (pipe_geometry) {
        deactivate();
        activate_triangles();
        auto [V, C, T] = get_pos_col_tubes(index);

        spdlog::info("Updating tube cylinder geometry {} {} {}", V.cols(), C.cols(), T.cols());
        setTriangleBuffer(V.cast<float>().eval(), T.cast<unsigned int>().eval());
        setColorBuffer(C.cast<float>().eval());
    } else {
        deactivate();
        //activate_edges();
        auto [V, C, E] = get_pos_col_lines(index);

        setEdgeBuffer(V.cast<float>().eval(), E.cast<unsigned int>().eval());
        spdlog::info("Updating tube line geometry {} {} {} (E range {} {})", V.cols(), C.cols(), E.cols(), E.minCoeff(), E.maxCoeff());
        setColorBuffer(C.cast<float>().eval());
    }
}
TubeMeshConstructorGui::TubeMeshConstructorGui(
  const std::vector<Particles> &particles,
  const std::vector<int> &active_indices,
  const bool &show_all_particles,
  Magnum::SceneGraph::DrawableGroup3D &draw_group) : TubeMeshConstructor(particles, active_indices, show_all_particles), mtao::opengl::MeshDrawable<Magnum::Shaders::Phong>(*this, _phong_shader, draw_group), _phong_shader(Magnum::Shaders::Phong::Flag::VertexColor) {}

bool TubeMeshConstructorGui::gui() {
    {

        static const auto items = mtao::visualization::imgui::utils::strs_to_charPtr(ScalarFunctionModeNames);
        int m = static_cast<char>(scalar_function_mode);
        if (ImGui::Combo("Type", &m, items.data(), items.size())) {
            scalar_function_mode = static_cast<ScalarFunctionMode>(char(m));
            return true;
        }
    }
    if (ImGui::InputInt("Tail size", &tail_size)) {
        tail_size = std::max<int>(2, tail_size);
        return true;
    }
    if (ImGui::Checkbox("Tube Geometry", &pipe_geometry)) {
        pipe_geometry = true;
        return true;
    }
    if (ImGui::InputInt("Tube subdivision", &tube_subdivisions)) {
        tube_subdivisions = std::max<int>(3, tube_subdivisions);
        return true;
    }
    if (ImGui::InputFloat("Tube radius", &tube_radius)) {
        return true;
    }

    if (ImGui::TreeNode("Colormap settings")) {
        if (_colmap_widget.gui()) {
            return true;
        }
        ImGui::TreePop();
    }

    return false;
}
nlohmann::json TubeMeshConstructorGui::config() const {
    nlohmann::json js;

    {// read off colormap settings
        auto &colmap_js = js["colormap"];
        colmap_js["type"] =
          mtao::visualization::imgui::ColorMapSettingsWidget::ColorMapNames[int(_colmap_widget.get_type())];
        {
            colmap_js["scale"] = _colmap_widget.scale;
            colmap_js["shift"] = _colmap_widget.shift;
        }
    }

    js["tail_size"] = tail_size;
    js["tube_radius"] = tube_radius;
    js["tube_subdivisions"] = tube_subdivisions;
    return js;
}
void TubeMeshConstructorGui::save_config(const std::filesystem::path &path) const {

    std::ofstream ofs(path);
    nlohmann::json js = config();
    ofs << js;
}
void TubeMeshConstructorGui::load_config(const std::filesystem::path &path) {
    std::ifstream ifs(path);
    nlohmann::json js;
    ifs >> js;
    load_config(js);
}

void TubeMeshConstructorGui::load_config(const nlohmann::json &js) {

    {// read off colormap settings
        const auto &colmap_js = js["colormap"];
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

    tail_size = js["tail_size"].get<int>();
    tube_radius = js["tube_radius"].get<float>();
    tube_subdivisions = js["tube_subdivisions"].get<int>();
}

mtao::ColVecs4d TubeMeshConstructorGui::colors_from_scalar(const mtao::VecXd &F) const {

    return _colmap_widget.get_rgba(F);
}

std::tuple<mtao::ColVecs3d, mtao::ColVecs4d, mtao::ColVecs3i> TubeMeshConstructorGui::get_pos_col_tubes(int index) const {
    auto [V, F, T] = get_pos_scalar_tubes(index);
    return { V, colors_from_scalar(F), T };
}
std::tuple<mtao::ColVecs3d, mtao::ColVecs4d, mtao::ColVecs2i> TubeMeshConstructorGui::get_pos_col_lines(int index) const {
    auto [V, F, E] = get_pos_scalar_lines(index);
    return { V, colors_from_scalar(F), E };
}
void TubeMeshConstructorGui::save_ply(int index, const std::string &path_format) const {
    auto [V, C, T] = get_pos_col_tubes(index);
    std::string path = fmt::vformat(path_format, fmt::make_format_args(index));
    mtao::geometry::mesh::write_plyD(V, C.topRows<3>().eval(), T, path);
}
