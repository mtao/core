#include <algorithm>
#include <catch2/catch.hpp>
#include <iostream>
#include <iterator>
#include <mtao/geometry/mesh/edges_to_plcurves.hpp>

using namespace mtao::geometry::mesh;

TEST_CASE("ClosedCurves", "[edges_to_plcurves]") {
    auto process = [](const mtao::ColVecs2i& E) {
        std::cout << E << std::endl;
        auto r = edge_to_plcurves(E);
        for (auto&& [vec, closedness] : r) {
            std::copy(vec.begin(), vec.end(),
                      std::ostream_iterator<int>(std::cout, ","));
            REQUIRE(vec.size() == E.cols());
            REQUIRE(closedness == true);
            if (closedness) {
                std::cout << "closed" << std::endl;
            } else {
                std::cout << "open" << std::endl;
            }
        }
    };
    {
        mtao::ColVecs2i E(2, 3);
        E.col(0) << 0, 1;
        E.col(1) << 1, 2;
        E.col(2) << 2, 0;
        process(E);
    }
    {
        mtao::ColVecs2i E(2, 4);
        E.col(0) << 0, 1;
        E.col(1) << 1, 2;
        E.col(2) << 2, 3;
        E.col(3) << 3, 0;
        process(E);
    }
}
TEST_CASE("OpenCurves", "[edges_to_plcurves]") {
    auto process = [](const mtao::ColVecs2i& E) {
        std::cout << "Edges: \n";
        std::cout << E << std::endl;
        auto r = edge_to_plcurves(E,false);
        for (auto&& [vec, closedness] : r) {
            std::copy(vec.begin(), vec.end(),
                      std::ostream_iterator<int>(std::cout, ","));
            REQUIRE(closedness == false);
            // max is for the first case only!
            REQUIRE(vec.size() == std::max<size_t>(2,E.cols()));
            if (closedness) {
                std::cout << "closed" << std::endl;
            } else {
                std::cout << "open" << std::endl;
            }
        }
    };
    {
        mtao::ColVecs2i E(2, 1);
        E.col(0) << 0, 1;
        process(E);
    }
    {
        mtao::ColVecs2i E(2, 2);
        E.col(0) << 0, 1;
        E.col(1) << 1, 2;
        process(E);
    }
    {
        mtao::ColVecs2i E(2, 4);
        E.col(0) << 0, 1;
        E.col(1) << 1, 2;
        E.col(2) << 2, 3;
        E.col(3) << 3, 4;
        process(E);
    }
}
