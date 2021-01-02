#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/mesh/edges_to_polygons.hpp>

using namespace mtao::geometry::mesh;

TEST_CASE("Polygon from edges", "[winding_number, holes]") {
    mtao::ColVecs2d V(2, 7);
    V.col(0) = mtao::Vec2d(0.0, 0.0);
    V.col(1) = mtao::Vec2d(1.0, 0.0);
    V.col(2) = mtao::Vec2d(1.0, 1.0);
    V.col(3) = mtao::Vec2d(0.0, 1.0);
    V.col(4) = mtao::Vec2d(0.5, 0.5);
    V.col(5) = mtao::Vec2d(0.6, 0.5);
    V.col(6) = mtao::Vec2d(0.4, 0.5);

    mtao::ColVecs2i E(2, 7);
    E.col(0) << 0, 1;
    E.col(1) << 1, 2;
    E.col(2) << 2, 3;
    E.col(3) << 3, 0;
    E.col(4) << 4, 5;
    E.col(5) << 5, 6;
    E.col(6) << 6, 4;

    auto loops = edges_to_polygons(V, E);
    for (auto &&loop : loops) {
        spdlog::info("Loop {}", fmt::join(loop, ","));
        for (auto &&h : loop.holes) {
            spdlog::info("----hole{}", fmt::join(h, ","));
        }
    }
}
