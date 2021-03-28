#include <iostream>
#include <mtao/geometry/mesh/boundary_elements.h>
#include <mtao/geometry/mesh/boundary_elements.h>
#include <mtao/geometry/mesh/boundary_facets.h>
#include <catch2/catch.hpp>

using namespace mtao::geometry::mesh;

TEST_CASE("tet_boundary_elements", "[boundary_elements]") {

    mtao::ColVectors<int, 4> T(4, 5);
    for (int i = 0; i < T.cols(); ++i) {
        T.col(i) = mtao::Vector<int, 4>(T.cols(), T.cols() + 1, i % (T.cols()), (i + 1) % (T.cols()));
    }
    auto F = mtao::geometry::mesh::boundary_facets(T);


    auto boundary_edges = boundary_element_indices(T, F);

    // each tet has 2 outward facing faces
    REQUIRE(boundary_edges.size() == 2 * T.cols());

    // make sure none of teh boundary triangles contain the 5 6 edge
    for (auto &&be : boundary_edges) {
        auto f = F.col(be);
        int count = 0;
        for (auto &&v : f) {
            if (v == T.cols() || v == T.cols() + 1) {
                count++;
            }
        }
        CHECK(count < 2);
    }
}

TEST_CASE("tri_boundary_elements", "[boundary_elements]") {
    mtao::ColVectors<int, 3> T(3, 5);
    for (int i = 0; i < T.cols(); ++i) {
        T.col(i) = mtao::Vector<int, 3>(T.cols(), i, (i + 1) % T.cols());
    }

    auto E = mtao::geometry::mesh::boundary_facets(T);

    auto boundary_edges = boundary_element_indices(T, E);

    REQUIRE(boundary_edges.size() == T.cols());
    for (auto &&be : boundary_edges) {
        auto e = E.col(be);
        CHECK(e.maxCoeff() != T.cols());
    }
}

TEST_CASE("cell_boundary_elements", "[boundary_elements]") {
    std::vector<std::map<int, bool>> cells(2);

    cells[0] = { { 0, false }, { 1, false }, { 2, false } };
    cells[1] = { { 3, false }, { 1, true }, { 2, false } };


    auto boundary_edges = boundary_element_indices(cells);

    REQUIRE(boundary_edges.size() == 3);
    CHECK(boundary_edges.contains(0));
    CHECK(boundary_edges.contains(2));
    CHECK(boundary_edges.contains(3));
}
