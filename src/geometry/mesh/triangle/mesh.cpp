#include "mtao/geometry/mesh/triangle/mesh.h"
#include "mtao/eigen_utils.h"
#include "mtao/geometry/circumcenter.h"
#include "mtao/geometry/barycentric.hpp"
#include "mtao/geometry/volume.h"
#include "mtao/logging/timer.hpp"
using namespace mtao::logging;
namespace mtao::geometry::mesh::triangle {
Mesh::Mesh(const std::tuple<mtao::ColVectors<REAL, 2>, mtao::ColVectors<int, 2>> &VE) : Mesh(std::get<0>(VE), std::get<1>(VE)) {
    make_circumcenters();
}
void Mesh::make_circumcenters() {
    if (F.cols() == 0) {
        C = std::nullopt;
    }
    C = mtao::geometry::circumcenters(V, F);
}

void Mesh::translate(const mtao::Vector<REAL, 2> &t) {
    V.colwise() += t;
    H.colwise() += t;
}
void Mesh::scale(const mtao::Vector<REAL, 2> &s) {
    V.array().colwise() *= s.array();
    H.array().colwise() *= s.array();
}
Mesh::Mesh(const mtao::ColVectors<REAL, 2> V,
           const mtao::ColVectors<int, 2> E,
           const mtao::ColVectors<int, 3> F) : V(V), E(E), F(F) {
    fill_attributes();

    make_circumcenters();
}

void Mesh::fill_attributes() {
    VA = mtao::VectorX<int>::Zero(V.cols());
    EA = mtao::VectorX<int>::Zero(E.cols());
}
Mesh combine(const std::vector<Mesh> &components) {
    Mesh m;

    std::vector<int> vertex_offsets;
    std::vector<int> edge_offsets;
    std::tie(m.V, vertex_offsets) = vector_hstack<decltype(m.V)>(components, [](const Mesh &m) { return m.V; });
    std::tie(m.H, std::ignore) = vector_hstack<decltype(m.V)>(components, [](const Mesh &m) { return m.H; });
    std::tie(m.E, edge_offsets) = vector_hstack<decltype(m.E)>(components, [](const Mesh &m) { return m.E; });

    for (size_t i = 0; i < components.size(); ++i) {
        auto mo = edge_offsets[i];
        auto mn = edge_offsets[i + 1];
        m.E.block(0, mo, 2, mn - mo).array() += vertex_offsets[i];
    }

    std::tie(m.VA, std::ignore) = vector_vstack<decltype(m.VA)>(components, [](const Mesh &m) { return m.VA; });
    std::tie(m.EA, std::ignore) = vector_vstack<decltype(m.EA)>(components, [](const Mesh &m) { return m.EA; });


    return m;
}


mtao::VectorX<bool> Mesh::verify_delauney() const {
    mtao::VectorX<bool> valid(F.cols());
    auto &&mC = *C;

#pragma omp parallel for
    for (int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        mtao::Matrix<REAL, 2, 3> T;
        for (int i = 0; i < 3; ++i) {
            T.col(i) = V.col(f(i));
        }
        auto B = mtao::geometry::barycentric_simplicial(T, mC.col(i).eval());
        valid(i) = (B.minCoeff() > 1e-5);
        //auto c = mC.col(i);
        /*
               for(int i = 0; i < 3; ++i) {
               auto u = V.col(f((i+0)%3));
               auto v = V.col(f((i+1)%3));
               auto w = V.col(f((i+2)%3));

               auto avg = ((u + v) / 2).eval();

               valid(i) = (w - avg).dot(c-avg) > 0;


               }
               */
    }

    int count = valid.rows() - valid.count();
    if (count > 0) {
        warn() << "Nonzero delauney failures: " << count;
    }


    return valid;
}
}// namespace mtao::geometry::mesh::triangle
