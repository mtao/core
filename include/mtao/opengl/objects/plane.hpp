#pragma once
#include "mtao/opengl/objects/mesh.h"
#include <Eigen/Geometry>
#include <Eigen/src/Core/util/Memory.h>


namespace mtao::opengl::objects {
class Plane : public Mesh<3> {
  public:
    Plane();
    Plane(const Vec4f &p);
    void set_equation(const Vec4f &P);
    // equation lacks a certain
    void set_equation(const Vec4f &P, const Vec3f &U);
    void set_plane(const Vec3f &O, const Vec3f &U, const Vec3f &V);
    void set_plane(const Mat3f &D);

    mtao::Vec4f get_equation() const;
    // origin, u, v
    const mtao::Mat3f &get_plane() const;
    bool gui();
    // if data changes call this:w
    void update();

    auto O() const { return data.col(0); }
    auto U() const { return data.col(1); }
    auto V() const { return data.col(2); }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  private:
    auto O() { return data.col(0); }
    auto U() { return data.col(1); }
    auto V() { return data.col(2); }
    mtao::Mat3f data;
    bool use_equation = false;
};
}// namespace mtao::opengl::objects
