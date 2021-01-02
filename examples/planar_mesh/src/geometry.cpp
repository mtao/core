#include "geometry.h"
#include <mtao/geometry/mesh/sphere.hpp>
#include <mtao/logging/timer.hpp>
using namespace mtao::logging;
using namespace mtao::geometry::mesh::triangle;
Shape::~Shape() {}
//Square::~Square() {}
//Transform::~Transform() {}
//Translate::~Translate() {}
//Scale::~Scale() {}

void Shape::set_vert_indices(const std::map<int, int> &map) {
    vert_marker_map = map;
}
int Shape::get_vert_index(int i) const {
    if (auto it = vert_marker_map.find(i); it != vert_marker_map.end()) {
        return it->second;
    } else {
        return i;
    }
}
void Shape::set_edge_indices(const std::map<int, int> &map) {
    edge_marker_map = map;
}
void Shape::apply_edge_markers(mtao::VectorX<int> &V) const {
    V.noalias() = V.unaryExpr([&](int i) { return get_edge_index(i); });
}
void Shape::apply_vert_markers(mtao::VectorX<int> &V) const {
    V.noalias() = V.unaryExpr([&](int i) { return get_vert_index(i); });
}
int Shape::get_edge_index(int i) const {
    if (auto it = edge_marker_map.find(i); it != edge_marker_map.end()) {
        return it->second;
    } else {
        return i;
    }
}
Transform::Transform(const std::shared_ptr<Shape> &s) : shape(s) {}
int Transform::get_vert_index(int i) const {
    return shape->get_vert_index(Shape::get_vert_index(i));
}
int Transform::get_edge_index(int i) const {
    return shape->get_edge_index(Shape::get_edge_index(i));
}

Translate::Translate(const std::shared_ptr<Shape> &sh, REAL sx, REAL sy) : Transform(sh), t(sx, sy) {}
Mesh Translate::boundary_geometry() {
    Mesh m = shape->boundary_geometry();
    m.translate(t);
    return m;
}

Scale::Scale(const std::shared_ptr<Shape> &sh, REAL sx, REAL sy) : Transform(sh), s(sx, sy) {}
Scale::Scale(const std::shared_ptr<Shape> &sh, REAL s) : Transform(sh), s(s, s) {}
Mesh Scale::boundary_geometry() {
    Mesh m = shape->boundary_geometry();
    m.scale(s);
    return m;
}

Square::Square(REAL r, int nx, int ny) : radius(r), nx(nx), ny(ny) {}

Sphere::Sphere(REAL r, int vertcount) : radius(r), vertcount(vertcount) {}

Mesh Sphere::boundary_geometry() {
    Mesh m = mtao::geometry::mesh::sphere<REAL, 2>(vertcount);
    m.V *= radius;
    m.VA.setConstant(1);
    m.EA.setConstant(1);
    apply_vert_markers(m.VA);
    apply_edge_markers(m.EA);
    return m;
}

Mesh Square::boundary_geometry() {
    using Vec2 = mtao::Vector<REAL, 2>;
    Mesh m;
    auto &&V = m.V;
    auto &&E = m.E;
    auto &&VA = m.VA;
    auto &&EA = m.EA;


    int size = 2 * (nx + ny);
    int stride = nx + ny;

    V.resize(2, size);
    E.resize(2, size);
    m.fill_attributes();

    REAL dx = 1.0 / nx;
    REAL dy = 1.0 / ny;
    VA.setConstant(get_vert_index(0));
    EA.setConstant(get_edge_index(0));

    for (int i = 0; i < nx; ++i) {
        V.col(i).template topRows<2>() = Vec2(i * dx, 0);
        V.col(stride + nx - i - 1).template topRows<2>() = Vec2((i + 1) * dx, 1);
        EA(i) = get_edge_index(1);
        EA(stride + nx - i - 1) = get_edge_index(3);
        VA(i) = get_vert_index(1);
        VA(stride + nx - i - 1) = get_vert_index(1);
    }
    for (int i = 0; i < ny; ++i) {
        V.col(nx + i).template topRows<2>() = Vec2(1, i * dy);
        V.col(V.cols() - i - 1).template topRows<2>() = Vec2(0, (i + 1) * dy);
        EA(nx + i) = get_edge_index(2);
        EA(V.cols() - i - 1) = get_edge_index(4);
        VA(nx + i) = get_vert_index(1);
        VA(V.cols() - i - 1) = get_vert_index(1);
    }
    for (int i = 0; i < E.cols(); ++i) {
        auto e = E.col(i);
        e(0) = i;
        e(1) = (i + 1) % E.cols();
    }

    //V.array() -= .5;
    //V = 4 * V;
    V.array() *= 2;
    V.array() -= 1;
    V.array() *= radius;

    return m;
}
