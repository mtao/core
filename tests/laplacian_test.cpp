#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/mesh/laplacian.hpp>
#include <mtao/simulation/hexahedral/laplacian.hpp>
#include <mtao/simulation/simplicial/laplacian.hpp>

using namespace mtao::simulation::simplicial;
using namespace mtao::geometry::mesh;

TEST_CASE("Lap2", "[laplacian]") {
    return;
    mtao::ColVecs2d V(2, 3);
    V.col(0) = mtao::Vec2d(0.0, 1.0);
    V.col(1) = mtao::Vec2d(0.8, 0.0);
    V.col(2) = mtao::Vec2d(-.8, 0.0);

    // V.col(0) = mtao::Vec2d(0.0,0.0);
    // V.col(1) = mtao::Vec2d(1.,0.0);
    // V.col(2) = mtao::Vec2d(0.,1.);
    mtao::ColVecs3i F(3, 1);
    F << 0, 1, 2;

    auto L = laplacian(V, F);
    std::cout << L << std::endl;
    auto L2 = cot_laplacian(V, F);
    std::cout << L2 << std::endl;

    REQUIRE(1 == 1);
}
TEST_CASE("Lap3", "[laplacian]") {
    mtao::ColVecs3d V(3, 4);

    V.col(0) = mtao::Vec3d(0.0, 0.0, 0.0);
    V.col(1) = mtao::Vec3d(1.0, 0.0, 0.0);
    V.col(2) = mtao::Vec3d(0.0, 1.0, 0.0);
    V.col(3) = mtao::Vec3d(0.0, 0.0, 1.0);
    mtao::ColVecs4i F(4, 1);
    F << 0, 1, 2, 3;

    auto L = laplacian(V, F);
    std::cout << L << std::endl;

    REQUIRE(1 == 1);
}
TEST_CASE("hex", "[laplacian,hexahedral]") {
    return;
    auto L2 = mtao::simulation::hexahedral::laplacian_stencil<float, 2>();
    std::cout << L2 << std::endl;
    std::cout << L2.colwise().sum() << std::endl;
    auto L3 = mtao::simulation::hexahedral::laplacian_stencil<float, 3>();
    std::cout << L3 << std::endl;
    std::cout << L3.colwise().sum() << std::endl;
}
