#include "mtao/opengl/objects/plane.hpp"
#include "mtao/geometry/mesh/shapes/cube.hpp"
#include <imgui.h>
namespace mtao::opengl::objects {
Plane::Plane() : Plane(Vec4f::Unit(2)) {}
Plane::Plane(const Vec4f &p) {
    set_equation(p);
}
// equation lacks a certain
void Plane::set_equation(const Vec4f &P) {
    auto N = P.head<3>();
    int minind;
    float min_val = std::numeric_limits<float>::max();
    N.cwiseAbs().minCoeff(&minind);
    for (int j = 0; j < 3; ++j) {
        float v = std::abs(N(j));
        if (v < min_val) {
            minind = j;
            min_val = v;
        }
    }
    Vec3f U = N.cross(Vec3f::Unit(minind));
    set_equation(P, U);
}
void Plane::set_equation(const Vec4f &P, const Vec3f &U) {
    auto N = P.head<3>();
    float N2 = N.squaredNorm();
    // N \cdot O + P(3) = 0
    // N \cdot ( N / N.normSquared() * P(3)
    auto O = -P(3) * N / N2;
    N2 = std::sqrt(std::sqrt(N2));
    auto V = N.cross(U).eval();

    set_plane(O, N2 * U.normalized(), N2 * V.normalized());
}
void Plane::set_plane(const Vec3f &O, const Vec3f &U, const Vec3f &V) {
    data.col(0) = O;
    data.col(1) = U;
    data.col(2) = V;
    update();
}

void Plane::set_plane(const Mat3f &D) {
    data = D;
    update();
}

void Plane::update() {
    auto O = this->O();
    auto U = this->U();
    auto V = this->V();

    auto [P, F] = mtao::geometry::mesh::shapes::square<float, 3>();
    P.array() = P.array() * 2.f - 1.f;

    P = ((U * P.row(0) + V * P.row(1)).colwise() + O).eval();

    Mesh<3>::setVertexBuffer(P);
    Mesh<3>::setTriangleBuffer(F.template cast<unsigned int>());
}

mtao::Vec4f Plane::get_equation() const {
    mtao::Vec4f P;
    auto N = P.head<3>();
    N = U().cross(V());
    // N.dot(O) + P(3) = 0
    P(3) = -N.dot(O());
    return P;
}
bool Plane::gui() {

    bool changed = false;
    ImGui::Checkbox("Use Equation", &use_equation);
    if (use_equation) {
        mtao::Vec4f P = get_equation();
        changed |= ImGui::InputFloat4("Equation", P.data());

        auto N = P.head<3>();
        float norm = N.norm();
        if (ImGui::InputFloat("Norm", &norm)) {
            N = N.normalized() * norm;
            changed = true;
        }


        //std::cout << P.transpose() << std::endl;
        if (changed) {
            set_equation(P);
            //std::cout << get_equation().transpose() << std::endl;
            //std::cout << std::endl;
        }

    } else {
        changed |= ImGui::InputFloat3("Origin", O().data());
        changed |= ImGui::InputFloat3("U", U().data());
        changed |= ImGui::InputFloat3("V", V().data());
    }
    if (changed) {
        update();
    }
    return changed;
}
}// namespace mtao::opengl::objects
