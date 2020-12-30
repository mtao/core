
#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/volume.hpp>

#include <mtao/geometry/mesh/earclipping.hpp>
TEST_CASE("earclipping_orientation", "[earclipping]") {
    mtao::ColVectors<float, 2> V(2, 5);
    std::vector loop{ 0, 1, 2, 3, 4 };

    V.col(0) = mtao::Vec2f(0., 0.);
    V.col(1) = mtao::Vec2f(1., 0.);
    V.col(2) = mtao::Vec2f(1., 1.);
    V.col(3) = mtao::Vec2f(0., 1.);
    V.col(4) = mtao::Vec2f(.5f, .5f);


    auto T = mtao::geometry::mesh::earclipping(V, loop);

    auto vols = mtao::geometry::volumes(V, T);

    std::cout << vols.transpose() << std::endl;

    CHECK(vols.minCoeff() > 0);
    loop = { 4, 3, 2, 1, 0 };
    T = mtao::geometry::mesh::earclipping(V, loop);

    vols = mtao::geometry::volumes(V, T);
    std::cout << vols.transpose() << std::endl;
    CHECK(vols.minCoeff() > 0);
}
