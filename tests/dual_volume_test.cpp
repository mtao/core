#include <iostream>
#include <mtao/geometry/mesh/dual_volumes.hpp>
#include <catch2/catch.hpp>
#include <iterator>
#include <iostream>
#include <algorithm>



TEST_CASE("DualVolume2","[geometry,mesh]") {
    mtao::ColVecs2d V(2,4);
    mtao::ColVecs2i E(2,3);
    V.col(0) << 0.,0.;
    V.col(1) << 1.,0.;
    V.col(2) << 1.5,0.;
    V.col(3) << 2.,0.;

    E.col(0) << 0,1;
    E.col(1) << 1,2;
    E.col(2) << 2,3;
    auto DV = mtao::geometry::mesh::dual_volumes(V,E);
    CHECK(DV(0) == .5);
    CHECK(DV(1) == .75);
    CHECK(DV(2) == .5);
    CHECK(DV(3) == .25);

}


TEST_CASE("DualVolume3","[geometry,mesh]") {
    mtao::ColVecs2d V(2,4);
    mtao::ColVecs3i F(3,2);
    V.col(0) << 0.,0.;
    V.col(1) << 1.,0.;
    V.col(2) << 0,1.;
    V.col(3) << 1.,1.;

    F.col(0) << 0,1,2;
    F.col(1) << 1,3,2;
    auto DV = mtao::geometry::mesh::dual_volumes(V,F);
    CHECK(DV(0) == Approx(1/6.));
    CHECK(DV(1) == Approx(2/6.));
    CHECK(DV(2) == Approx(2/6.));
    CHECK(DV(3) == Approx(1/6.));

}

