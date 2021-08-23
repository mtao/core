#include "mtao/opengl/objects/sphere.hpp"
#include "mtao/geometry/mesh/shapes/sphere.hpp"
#include <imgui.h>
namespace mtao::opengl::objects {
Sphere::Sphere(float rad, int subdivs) : _radius(rad), _subdivisions(subdivs) {
    update();
}

void Sphere::set_radius(float rad) {
    _radius = rad;
}
void Sphere::set_subdivisions(int sub) {
    _subdivisions = sub;
}
void PositionedSphere::set_center(const mtao::Vec3f &c) {
    _center = c;
}
void Sphere::update() {
    auto [V, F] = geometry::mesh::shapes::sphere<float>(_subdivisions);
    V *= _radius;

    Mesh<3>::setTriangleBuffer(V, F.template cast<unsigned int>());
}

bool Sphere::gui() {
    bool changed = false;

    changed |= ImGui::InputFloat("Radius", &_radius);
    changed |= ImGui::InputInt("Subdivisions", &_subdivisions);
    if (changed) {
        update();
    }
    return changed;
}

PositionedSphere::PositionedSphere(const mtao::Vec3f &p, float rad, int subdivs) : Sphere(rad, subdivs), _center(p) {}

void PositionedSphere::update() {
    auto [V, F] = geometry::mesh::shapes::sphere<float>(subdivisions());
    V *= radius();
    V.colwise() += _center;

    Mesh<3>::setTriangleBuffer(V, F.template cast<unsigned int>());
}

bool PositionedSphere::gui() {
    bool changed = false;

    changed |= ImGui::InputFloat3("Center", _center.data());
    // try to change first here because the Sphere gui might also do an update as well
    if (changed) {
        update();
    }
    changed |= Sphere::gui();

    return changed;
}
}// namespace mtao::opengl::objects
