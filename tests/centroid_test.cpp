
#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/centroid.hpp>

using namespace mtao::geometry;

TEST_CASE("triangle_centroid", "[centroid]") {
    mtao::ColVecs2d V(2, 3);

    mtao::ColVecs2i E(2, 3);
    E.col(0) << 0, 1;
    E.col(1) << 1, 2;
    E.col(2) << 2, 0;
    std::map<int, bool> dat{{0, false}, {1, false}, {2, false}};
    auto C = V.rowwise().mean();

    auto check = [&]() {
        auto C2 = curve_centroid(V, {0, 1, 2});
        auto C3 = centroid(V, E, dat);

        CHECK(C(0) == Approx(C2(0)));
        CHECK(C(1) == Approx(C2(1)));
        CHECK(C(0) == Approx(C3(0)));
        CHECK(C(1) == Approx(C3(1)));
    };

    V.col(0) = mtao::Vec2d(0.0, 0.0);
    V.col(1) = mtao::Vec2d(1.0, 0.0);
    V.col(2) = mtao::Vec2d(1.0, 1.0);
    check();

    V.col(0) = mtao::Vec2d(2.1, 3.0);
    V.col(1) = mtao::Vec2d(-1.0, 0.1);
    V.col(2) = mtao::Vec2d(0.0, 1.0);
    check();
}
