#pragma once
#include "particles.hpp"
#include <mtao/opengl/objects/plane.hpp>
#include <mtao/opengl/objects/sphere.hpp>
#include <igl/AABB.h>

struct Filter {
    using BoolVec = Eigen::Array<bool, Eigen::Dynamic, 1>;

    virtual BoolVec particle_mask(const Particles &p) const = 0;
    std::vector<int> selected_particles(const Particles &p) const;
    virtual bool gui() { return false; }
};

struct PlaneFilter : public Filter
  , public mtao::opengl::objects::Plane {

    BoolVec particle_mask(const Particles &p) const override;
    bool gui() override;
};

struct SphereFilter : public Filter
  , public mtao::opengl::objects::PositionedSphere {

    BoolVec particle_mask(const Particles &p) const override;
    bool gui() override;
};

struct MeshFilter : public Filter {
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::AABB<Eigen::MatrixXf, 3> aabb;
    float mesh_distance = .1;
    MeshFilter(const Eigen::MatrixXf &V, const Eigen::MatrixXi &F);
    BoolVec particle_mask(const Particles &p) const override;
    bool gui() override;
};

struct IntersectionFilter : public Filter {
    std::vector<std::tuple<std::string, std::shared_ptr<Filter>, bool>> filters;
    BoolVec particle_mask(const Particles &p) const override;
    bool gui() override;
};
