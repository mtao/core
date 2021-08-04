#pragma once
#include "mtao/opengl/objects/mesh.h"
#include <mtao/types.hpp>


namespace mtao::opengl::objects {
struct Sphere : public Mesh<3> {
  public:
    Sphere(float rad = 1, int subdivs = 2);

    bool gui();
    // if data changes call this
    virtual void update();
    float radius() const { return _radius; }
    int subdivisions() const { return _subdivisions; }

  private:
    float _radius = 1;
    int _subdivisions = 2;
};

struct PositionedSphere : public Sphere {
  public:
    PositionedSphere(const mtao::Vec3f &p = mtao::Vec3f::Zero(), float rad = 1, int subdivs = 2);

    bool gui();
    virtual void update() override;
    const mtao::Vec3f &center() const { return _center; }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  private:
    mtao::Vec3f _center;
};
}// namespace mtao::opengl::objects
