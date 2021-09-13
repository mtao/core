#pragma once
#include "tube_mesh_constructor.hpp"
#include <mtao/visualization/imgui/colormap_settings_widget.hpp>
#include <filesystem>
#include "particles.hpp"
#include <mtao/opengl/objects/mesh.h>
#include <mtao/opengl/drawables.h>
#include <Magnum/Shaders/Phong.h>
#include <nlohmann/json.hpp>




struct TubeMeshConstructorGui : public TubeMeshConstructor
  , mtao::opengl::objects::Mesh<3>
  , mtao::opengl::MeshDrawable<Magnum::Shaders::Phong> {
    TubeMeshConstructorGui(
      const std::vector<Particles> &particles,
      const std::vector<int> &active_indices,
      const bool &show_all_particles,
    const mtao::visualization::imgui::ColorMapSettingsWidget &colmap_widget,
      Magnum::SceneGraph::DrawableGroup3D &draw_group);
    bool gui();
    bool color_gui();
    void save(const std::string &filename);
    void update(int index);
    const mtao::visualization::imgui::ColorMapSettingsWidget &colmap_widget() const { return _colmap_widget; }

    mtao::ColVecs4d colors_from_scalar(const mtao::VecXd &f) const;

    std::tuple<mtao::ColVecs3d, mtao::ColVecs4d, mtao::ColVecs3i> get_pos_col_tubes(int index) const;
    std::tuple<mtao::ColVecs3d, mtao::ColVecs4d, mtao::ColVecs2i> get_pos_col_lines(int index) const;


    nlohmann::json config() const;
    void save_config(const std::filesystem::path &path) const;
    void load_config(const std::filesystem::path &path);
    void load_config(const nlohmann::json &js);

    void save_ply(int index, const std::string &path_format) const;

  private:
    const mtao::visualization::imgui::ColorMapSettingsWidget& _colmap_widget;
    Magnum::Shaders::Phong _phong_shader;
};
