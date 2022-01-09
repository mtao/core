#include <iostream>
#include <mtao/geometry/cyclic_order.hpp>
#include <catch2/catch.hpp>
#include <iterator>
#include <iostream>
#include <algorithm>

using E = std::array<int, 2>;

bool is_cyclic_ordered(const std::vector<int> &indices) {


    for (size_t idx = 0; idx < indices.size(); ++idx) {
        if (int((indices[idx] + 1) % indices.size()) != indices[(idx + 1) % indices.size()]) {
            return false;
        }
    }

    return true;
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wempty-body"
TEST_CASE("Polygon", "[cyclic_order]") {
    for (int N = 3; N < 50; ++N) {
        std::stringstream ss;
        ss << "Polygon size " << N;
        SECTION(ss.str());
        mtao::ColVecs2d V(2, N);

        mtao::VecXd ang = mtao::VecXd::LinSpaced(N, 0, 2 * M_PI * (N - 1.) / N);
        V.row(0) = ang.array().cos().transpose();
        V.row(1) = ang.array().sin().transpose();


        auto order = mtao::geometry::cyclic_order(V);
        REQUIRE(is_cyclic_ordered(order));
    }
}
TEST_CASE("Polygon with Center", "[cyclic_order]") {
    for (int N = 3; N < 50; ++N) {
        std::stringstream ss;
        ss << "Polygon size " << N;
        SECTION(ss.str());
        mtao::ColVecs2d V(2, N);

        mtao::VecXd ang = mtao::VecXd::LinSpaced(N, 0, 2 * M_PI * (N - 1.) / N);
        V.row(0).head(N) = ang.array().cos().transpose();
        V.row(1).head(N) = ang.array().sin().transpose();


        auto order = mtao::geometry::cyclic_order(V);
        REQUIRE(is_cyclic_ordered(order));
    }
}
TEST_CASE("Near Axis Fan", "[cyclic_order]") {
    for (int N = 3; N < 50; ++N) {
        std::stringstream ss;
        SECTION(ss.str());
        mtao::ColVecs2d V(2, N);

        V.row(0).head(N).setConstant(1e-8);
        V.row(1).head(N) = mtao::VecXd::LinSpaced(N, 1, 1e10);

        auto order = mtao::geometry::cyclic_order(V);
        REQUIRE(is_cyclic_ordered(order));
    }
}

TEST_CASE("Near Diagonal Fan", "[cyclic_order]") {
    for (int N = 3; N < 50; ++N) {
        std::stringstream ss;
        SECTION(ss.str());
        mtao::ColVecs2d V(2, N);

        V.row(0).head(N) = mtao::VecXd::LinSpaced(N, 1, 1e10);
        V.row(1).head(N) = V.row(0).head(N).array() - .5;

        auto order = mtao::geometry::cyclic_order(V);
        REQUIRE(is_cyclic_ordered(order));
    }
}
#pragma GCC diagnostic pop
