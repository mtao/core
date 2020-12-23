#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/winding_number.hpp>

using namespace mtao::geometry;

TEST_CASE("winding numbers", "[winding_number]") {
    mtao::ColVecs2d V(2, 5);
    V.col(0) = mtao::Vec2d(0.0, 0.0);
    V.col(1) = mtao::Vec2d(1.0, 0.0);
    V.col(2) = mtao::Vec2d(1.0, 1.0);
    V.col(3) = mtao::Vec2d(0.0, 1.0);
    V.col(4) = mtao::Vec2d(0.5, 0.5);

    std::vector<int> curve{0, 1, 2, 3};

    mtao::ColVecs2i E(2, 4);
    E.col(0) << 0, 1;
    E.col(1) << 1, 2;
    E.col(2) << 2, 3;
    E.col(3) << 3, 0;

    std::map<int, bool> B;
    B[0] = true;
    B[1] = true;
    B[2] = true;
    B[3] = true;

    for (int i = 0; i < 20; ++i) {
        mtao::Vec2d p = mtao::Vec2d::Random();
        std::cout << p.transpose() << ": ";
        std::cout << winding_number(V, curve, p) << " ";
        std::cout << "BLoop: " << mesh_winding_number(V, E, B, p) << " ";
        std::cout << "Just Edges: " << mesh_winding_number(V, E, p) << std::endl;
    }
}
