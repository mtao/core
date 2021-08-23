#pragma once
#include <filesystem>
#include "particles.hpp"
#include <mtao/opengl/objects/mesh.h>
#include <mtao/opengl/drawables.h>
#include <Magnum/Shaders/Phong.h>
#include <nlohmann/json.hpp>


struct TubeMeshConstructor {

  public:
    TubeMeshConstructor(
      const std::vector<Particles> &particles,

      const std::vector<int> &active_indices,
      const bool &show_all_particles);
    enum ScalarFunctionMode : char { All,
                                     Selected,
                                     Velocity,
                                     Density,
                                     END };
    static const std::array<std::string, int(ScalarFunctionMode::END)> ScalarFunctionModeNames;

    ScalarFunctionMode scalar_function_mode = ScalarFunctionMode::All;


    bool active() const {
        return bool(tail_size);
    }


    std::array<int, 2> valid_interval(int index) const;

    const Particles &frame_particles(int index) const;
    mtao::ColVecs3d frame_positions(int index) const;
    mtao::ColVecs3d frame_velocities(int index) const;
    mtao::VecXd frame_densities(int index) const;

    std::tuple<mtao::ColVecs3d, mtao::VecXd> get_pos_scalar(int index) const;
    std::vector<std::tuple<mtao::ColVecs3d, mtao::VecXd>> get_pos_scalar_tail(int index) const;

    std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs3i> get_pos_scalar_tubes(int index) const;
    std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs2i> get_pos_scalar_lines(int index) const;
    void set_scalar_function_mode(ScalarFunctionMode mode);

  private:
    const std::vector<Particles> &particles;

    const std::vector<int> &active_indices;
    const bool &show_all_particles;

  protected:
    int tail_size = 3;
    bool pipe_geometry = false;
    float tube_radius = .1;
    int tube_subdivisions = 3;
};


