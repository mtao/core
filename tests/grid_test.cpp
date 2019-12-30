#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/grid/grid.h>
#include <utility>
#include <numeric>

using namespace Catch::literals;



template <int D>
void grid_test() {
    using Grid = mtao::geometry::grid::GridD<float,D>;
    typename Grid::coord_type a;
    using Vec = typename mtao::Vector<float,D>;
    using Veci = typename mtao::Vector<int,D>;
    std::iota(a.begin(),a.end(),3);
    Grid g(a);
    auto bb = g.bbox();

    using VeciM = typename Eigen::Map<Veci>;

    Vec dx = 1.0 / (VeciM(a.data()).array()-1).template cast<float>();

    SECTION("BBox Check") {
        REQUIRE(bb.min().isApprox(Vec::Zero()));
        REQUIRE(bb.max().isApprox(Vec::Ones()));
    }

    SECTION("Check idempotence of local and global conversions") {
        mtao::geometry::grid::utils::multi_loop(g.shape(),[&](auto&& a) {
                REQUIRE(g.vertex(a).isApprox(dx.asDiagonal() * VeciM(a.data()).template cast<float>()));
                });

        REQUIRE(g.vertices().isApprox( g.world_coord(g.local_vertices()) ));
        REQUIRE(g.local_vertices().isApprox(g.local_coord(g.vertices())));
        SECTION("Check idempotence") {
            for(int i = 0; i < D; ++i) {
                REQUIRE(g.local_coord(g.world_coord(.5,i),i) == Approx( g.local_coord(g.world_coord(.5,i),i) ));
            }
        }
    }

}


TEST_CASE("Grid vertices 2D", "[grid]") {
    grid_test<2>();
}
TEST_CASE("Grid vertices 3D", "[grid]") {
    grid_test<3>();
}
