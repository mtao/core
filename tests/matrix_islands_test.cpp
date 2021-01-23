#include <iostream>
#include <mtao/eigen/matrix_islands.hpp>
#include <catch2/catch.hpp>

using namespace mtao::eigen;

TEST_CASE("Polygon angles", "[trigonometry]") {
    mtao::MatXd V(5,5);
    V.setZero();
    V(0,1) = 2;
    V(2,0) = 2;
    V(2,4) = 2;
    V(3,3) = 2;

    {
    auto isles = matrix_islands(V);

    REQUIRE(isles.size() == 2);
    if(isles[1].size() == 4) {
        std::swap(isles[0],isles[1]);
    }
    CHECK(*isles[1].begin() == 3);

    std::vector<int> vec0{isles[0].begin(),isles[0].end()};
    REQUIRE(vec0.size() == 4);
    CHECK(vec0[0] == 0);
    CHECK(vec0[1] == 1);
    CHECK(vec0[2] == 2);
    CHECK(vec0[3] == 4);
    }

    {
        Eigen::SparseMatrix<double> M = V.sparseView();
    auto isles = matrix_islands(M);

    REQUIRE(isles.size() == 2);
    if(isles[1].size() == 4) {
        std::swap(isles[0],isles[1]);
    }
    CHECK(*isles[1].begin() == 3);

    std::vector<int> vec0{isles[0].begin(),isles[0].end()};
    REQUIRE(vec0.size() == 4);
    CHECK(vec0[0] == 0);
    CHECK(vec0[1] == 1);
    CHECK(vec0[2] == 2);
    CHECK(vec0[3] == 4);
    }

}
