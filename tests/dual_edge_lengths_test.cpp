#include <iostream>
#include <mtao/geometry/mesh/boundary_facets.h>
#include <mtao/geometry/mesh/dual_edge_lengths.hpp>
#include <catch2/catch.hpp>
#include <iterator>
#include <algorithm>


TEST_CASE("EdgeDualVolume2", "[geometry,mesh]") {

    mtao::ColVecs2d V(2, 4);
    mtao::ColVecs2i E(2, 3);
    V.col(0) << 0., 0.;
    V.col(1) << 1., 0.;
    V.col(2) << 1.5, 0.;
    V.col(3) << 2., 0.;

    E.col(0) << 0, 1;
    E.col(1) << 1, 2;
    E.col(2) << 2, 3;


    mtao::RowVecXi VV(4);
    VV(0) = 0;
    VV(1) = 1;
    VV(2) = 2;
    VV(3) = 3;
    auto DV = mtao::geometry::mesh::dual_edge_lengths(V, E, VV);
    std::cout << DV << std::endl;
    CHECK(DV(0) == Approx(.5));
    CHECK(DV(1) == Approx(.75));
    CHECK(DV(2) == Approx(.5));
    CHECK(DV(3) == Approx(.25));
}

TEST_CASE("EdgeDualVolume3", "[geometry,mesh]") {

    mtao::ColVecs2d V(2, 4);
    mtao::ColVecs3i F(3, 2);
    V.col(0) << 0., 0.;
    V.col(1) << 1., 0.;
    V.col(2) << 0, 1.;
    V.col(3) << 1., 1.;

    F.col(0) << 0, 1, 2;
    F.col(1) << 1, 3, 2;
    mtao::ColVecs2i E = mtao::geometry::mesh::boundary_facets(F);
    std::cout << "Edges:\n"
              << E << std::endl;
    auto DV = mtao::geometry::mesh::dual_edge_lengths(V, F, E);
    CHECK(DV(0) == Approx(.5));
    CHECK(DV(1) == Approx(.5));
    CHECK(DV(2) == Approx(0.).margin(1e-3));
    CHECK(DV(3) == Approx(.5));
    CHECK(DV(4) == Approx(.5));
}
