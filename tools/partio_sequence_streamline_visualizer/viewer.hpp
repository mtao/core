#pragma once

#include <mtao/visualization/imgui/colormap_settings_widget.hpp>
#include <mtao/opengl/Window.h>
#include <mtao/opengl/objects/mesh.h>
#include <filesystem>
#include "filter.hpp"
#include <mtao/opengl/drawables.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Corrade/Utility/Arguments.h>
#include <variant>
#include <set>
class MeshViewer : public mtao::opengl::Window3 {
    std::filesystem::path base_dir;
    std::string relative_format = "frame_{}/particles.bgeo.gz";
    std::vector<Particles> particles;
    enum ParticleColorMode : char { All,
                                    Selected,
                                    Velocity,
                                    Density };
    ParticleColorMode particle_color_mode = ParticleColorMode::All;

    mtao::visualization::imgui::ColorMapSettingsWidget colmap_widget;


    std::vector<int> active_indices;
    bool show_all_particles = true;
    std::optional<int> tail_size;
    bool pipe_geometry = false;
    float tube_radius = .1;
    int tube_subdivisions = 3;

    int current_frame = -1;

    Magnum::Shaders::Flat3D _flat_shader;
    Magnum::Shaders::VertexColor3D _vcolor_shader;
    Magnum::Shaders::Phong _phong_shader;


    mtao::opengl::objects::Mesh<3> point_mesh;
    //mtao::opengl::MeshDrawable<Magnum::Shaders::Flat3D> *point_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor3D> *point_drawable = nullptr;

    mtao::opengl::objects::Mesh<3> tube_mesh;
    mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor3D> *tube_drawable = nullptr;

    mtao::opengl::objects::Mesh<3> mesh;
    //mtao::opengl::MeshDrawable<Magnum::Shaders::Flat3D> *point_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Phong> *mesh_drawable = nullptr;


    std::shared_ptr<SphereFilter> sphere_filter;
    std::shared_ptr<PlaneFilter> plane_filter;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Phong> *_plane_viewer = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Phong> *_sphere_viewer = nullptr;
    std::shared_ptr<MeshFilter> mesh_filter;
    std::shared_ptr<IntersectionFilter> intersection_filter;

    std::shared_ptr<PruneFilter> prune_filter;
    std::shared_ptr<RangeFilter> range_filter;

  public:
    MeshViewer(const Arguments &args);
    void gui() override;

    const Particles &frame_particles(int index) const;
    mtao::ColVecs3d frame_positions(int index) const;
    mtao::ColVecs3d frame_velocities(int index) const;

    std::tuple<mtao::ColVecs3d, mtao::VecXd> get_pos_scalar(int index) const;
    std::vector<std::tuple<mtao::ColVecs3d, mtao::VecXd>> get_pos_scalar_tail(int index) const;

    std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs3i> get_pos_scalar_tubes(int index) const;
    std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs2i> get_pos_scalar_lines(int index) const;
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
    void select_particles_from_sphere(bool set_active);
    void select_particles_from_mesh(bool set_active);
    void select_particles_from_range(bool set_active);
    void select_particles_from_prune(bool set_active);

    void select_particles_from_all(bool set_active);

    std::string frame_fmt() const;
    std::filesystem::path frame_path(int index) const;
    Particles frame_particles_from_disk(int index) const;

    std::filesystem::path mesh_path() const;
};
