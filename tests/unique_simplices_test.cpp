#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/mesh/unique_simplices.hpp>

using namespace mtao::geometry::mesh;

TEST_CASE("Edge sorting", "[unique_simplices]") {
    {
        mtao::ColVecs2i F(2, 2);
        F.col(0) << 0, 1;
        F.col(1) << 1, 0;

        auto FF = unique_simplices(F);
        REQUIRE(FF.cols() == 1);
        REQUIRE(FF(0, 0) == 0);
        REQUIRE(FF(1, 0) == 1);
    }

    {
        mtao::ColVecs2i F(2, 3);
        F.col(0) << 0, 1;
        F.col(1) << 1, 0;
        F.col(2) << 2, 0;

        auto FF = unique_simplices(F);
        REQUIRE(FF.cols() == 2);
        REQUIRE(FF(0, 0) == 0);
        REQUIRE(FF(1, 0) == 1);
        REQUIRE(FF(0, 1) == 2);
        REQUIRE(FF(1, 1) == 0);
    }
}
TEST_CASE("Triangle sorting", "[unique_simplices]") {
    {
        mtao::ColVecs3i F(3, 6);
        F.col(0) << 0, 1, 2;
        F.col(1) << 0, 2, 1;
        F.col(2) << 1, 0, 2;
        F.col(3) << 1, 2, 0;
        F.col(4) << 2, 1, 0;
        F.col(5) << 2, 0, 1;

        auto FF = unique_simplices(F);
        REQUIRE(FF.cols() == 1);
        REQUIRE(FF(0, 0) == 0);
        REQUIRE(FF(1, 0) == 1);
        REQUIRE(FF(2, 0) == 2);
    }
    {
        mtao::ColVecs3i F(3, 6);
        F.col(0) << 0, 2, 1;
        F.col(1) << 0, 1, 2;
        F.col(2) << 1, 0, 2;
        F.col(3) << 1, 2, 0;
        F.col(4) << 2, 1, 0;
        F.col(5) << 2, 0, 1;

        auto FF = unique_simplices(F);
        REQUIRE(FF.cols() == 1);
        REQUIRE(FF(0, 0) == 1);
        REQUIRE(FF(1, 0) == 0);
        REQUIRE(FF(2, 0) == 2);
    }

    {
        mtao::ColVecs3i F(3, 6);
        F.col(5) << 0, 2, 1;
        F.col(1) << 0, 1, 2;
        F.col(2) << 1, 0, 2;
        F.col(3) << 1, 2, 0;
        F.col(4) << 2, 1, 0;
        F.col(0) << 2, 0, 1;

        auto FF = unique_simplices(F);
        REQUIRE(FF.cols() == 1);
        REQUIRE(FF(0, 0) == 0);
        REQUIRE(FF(1, 0) == 1);
        REQUIRE(FF(2, 0) == 2);
    }
    {
        mtao::ColVecs3i F(3, 3);
        F.col(0) << 0, 1, 2;
        F.col(1) << 1, 0, 2;
        F.col(2) << 2, 0, 3;

        auto FF = unique_simplices(F);
        REQUIRE(FF.cols() == 2);
        REQUIRE(FF(0, 0) == 0);
        REQUIRE(FF(1, 0) == 1);
        REQUIRE(FF(2, 0) == 2);
        REQUIRE(FF(0, 1) == 2);
        REQUIRE(FF(1, 1) == 0);
        REQUIRE(FF(2, 1) == 3);
    }
}
