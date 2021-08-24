#pragma once

#include <mtao/opengl/Window.h>
#include <mtao/opengl/objects/mesh.h>
#include <filesystem>
#include "filter.hpp"
#include <mtao/opengl/drawables.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Corrade/Utility/Arguments.h>
#include <variant>
#include <set>
#include "tube_mesh_constructor_gui.hpp"
class MeshViewer : public mtao::opengl::Window3 {
    std::filesystem::path base_dir;
    std::string relative_format = "frame_{}/particles.bgeo.gz";
    std::vector<Particles> particles;


    std::vector<int> active_indices;
    bool show_all_particles = true;
    bool show_tubes = false;

    std::string config_path = "/tmp/fluidsim_streamlines_config.js";
    //std::string config_path = "./fluidsim_streamlines_config.js";

    int current_frame = -1;

    // [start,end)
    std::array<int, 2> filter_interval(int index) const;

    Magnum::Shaders::Flat3D _flat_shader;
    Magnum::Shaders::VertexColor3D _vcolor_shader;
    Magnum::Shaders::Phong _phong_shader;


    mtao::opengl::objects::Mesh<3> point_mesh;
    //mtao::opengl::MeshDrawable<Magnum::Shaders::Flat3D> *point_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor3D> *point_drawable = nullptr;


    mtao::opengl::objects::Mesh<3> mesh;
    //mtao::opengl::MeshDrawable<Magnum::Shaders::Flat3D> *point_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Phong> *mesh_drawable = nullptr;

    TubeMeshConstructorGui tube_mesh_gui;

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

    void save_settings(const std::filesystem::path &path) const;
    void load_settings(const std::filesystem::path &path);
    void save_settings() const;
    void load_settings();

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
    void select_particles(std::vector<int> &&indices, bool set_active, bool deactivate_tubes = true);

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
