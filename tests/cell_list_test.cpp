#
#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/grid/cell_list.hpp>

using namespace mtao::geometry::grid;

TEST_CASE("CellList2", "[cell_list]") {
    mtao::ColVecs2d V(2, 20);
    V.setRandom();

    V.array() += 1;
    V.array() /= 2;

    std::cout << V << std::endl << std::endl;
    CellList<double, 2> cell_list({{10, 10}});
    cell_list.construct(V);
    cell_list.offsets.loop([&](auto&& c, int val) {
        std::cout << c[0] << " " << c[1] << "] " << val << std::endl;
    });
    for (auto&& v : cell_list.particle_table) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    for (int j = 0; j < 20; ++j) {
        auto v = mtao::Vec2d::Random().eval();
        v.array() += 1;
        v /= 2;
        std::cout << " ======> " << v.transpose() << std::endl;

        cell_list.neighbor_walker(
            [&](int idx) { std::cout << V.col(idx).transpose() << std::endl; },
            v, .08);
    }
}
