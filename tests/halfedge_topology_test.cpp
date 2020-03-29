#include <catch2/catch.hpp>
#include <iostream>

#include "mtao/geometry/mesh/edge_tangents.hpp"
#include "mtao/geometry/mesh/halfedge.hpp"
#include "mtao/geometry/mesh/halfedge_fv_map.hpp"
#include "mtao/logging/logger.hpp"
using namespace mtao::logging;

namespace {
template <typename D, typename D2>
void check_matrix(const Eigen::MatrixBase<D>& A,
                  const Eigen::MatrixBase<D2>& B) {
    REQUIRE(A.cols() == B.cols());
    REQUIRE(A.rows() == B.rows());
    for (int i = 0; i < A.rows(); ++i) {
        for (int j = 0; j < A.cols(); ++j) {
            CHECK(A(i, j) == B(i, j));
        }
    }
}

template <typename D, typename D2>
void check_hem(const Eigen::MatrixBase<D>& V, const Eigen::MatrixBase<D2>& E) {
    mtao::ColVectors<float, 2> T =
        mtao::geometry::mesh::edge_tangents(V, E, true);
    std::map<std::array<int, 2>, std::tuple<int, bool>> tangent_map;
    for (int i = 0; i < E.cols(); ++i) {
        auto e = E.col(i);
        if (e(0) > e(1)) {
            tangent_map[std::array<int, 2>{{e(1), e(0)}}] = {i, true};
        } else {
            tangent_map[std::array<int, 2>{{e(0), e(1)}}] = {i, false};
        }
    }

    Eigen::MatrixXi hem_edges;
    using namespace mtao::geometry::mesh;
    {
        auto hem = EmbeddedHalfEdgeMesh<float, 2>::from_edges(V, E);
        hem.make_topology();
        hem_edges = hem.edges();
        auto C = hem.cell_halfedges();
    }
    {
        auto hem = EmbeddedHalfEdgeMesh<float, 2>::from_edges(V, E);
        hem.make_topology(tangent_map, T);
        check_matrix(hem_edges, hem.edges());
    }
}
}  // namespace

TEST_CASE("TriangleFan", "[halfedge]") {
    mtao::ColVectors<float, 2> V(2, 4);
    mtao::ColVectors<int, 2> E(2, V.cols() + V.cols() - 3);

    float dt = 2 * M_PI / V.cols();
    for (int i = 0; i < V.cols(); ++i) {
        auto v = V.col(i);
        v(0) = std::cos(i * dt);
        v(1) = std::sin(i * dt);
        auto e = E.col(i);
        e(0) = i;
        e(1) = (i + 1) % V.cols();
    }
    for (int i = 2; i < V.cols() - 1; ++i) {
        auto e = E.col(V.cols() + i - 2);
        e(0) = 0;
        e(1) = i;
    }
    check_hem(V, E);
}

TEST_CASE("Mail", "[halfedge]") {
    mtao::ColVectors<float, 2> V(2, 5);
    mtao::ColVectors<int, 2> E(2, 6);

    V.col(0) = mtao::Vec2f(0., 0.);
    V.col(1) = mtao::Vec2f(1., 0.);
    V.col(2) = mtao::Vec2f(1., 1.);
    V.col(3) = mtao::Vec2f(0., 1.);
    V.col(4) = mtao::Vec2f(.5f, .5f);

    E.col(0) = mtao::Vec2i(0, 1);
    E.col(1) = mtao::Vec2i(1, 2);
    E.col(2) = mtao::Vec2i(2, 3);
    E.col(3) = mtao::Vec2i(3, 0);
    E.col(4) = mtao::Vec2i(0, 4);
    E.col(5) = mtao::Vec2i(4, 1);

    check_hem(V, E);
}

TEST_CASE("SquareWithLine", "[halfedge]") {
    mtao::ColVectors<float, 2> V(2, 5);
    mtao::ColVectors<int, 2> E(2, 5);

    V.col(0) = mtao::Vec2f(0., 0.);
    V.col(1) = mtao::Vec2f(1., 0.);
    V.col(2) = mtao::Vec2f(1., 1.);
    V.col(3) = mtao::Vec2f(0., 1.);
    V.col(4) = mtao::Vec2f(.5f, .5f);

    E.col(0) = mtao::Vec2i(0, 1);
    E.col(1) = mtao::Vec2i(1, 2);
    E.col(2) = mtao::Vec2i(2, 3);
    E.col(3) = mtao::Vec2i(3, 0);
    E.col(4) = mtao::Vec2i(0, 4);

    check_hem(V, E);
}

