#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/eigen/sparse_block_diagonal_repmats.hpp>

using namespace mtao::eigen;

TEST_CASE("SimpleExample", "[sparse_block_repmats]") {
    Eigen::SparseMatrix<double> A(3, 3);
    A.coeffRef(1, 0) = 1;
    A.coeffRef(0, 1) = 2;
    A.coeffRef(2, 2) = 3;
    auto B = sparse_block_diagonal_repmats(A, 4);

    REQUIRE(4 * A.nonZeros() == B.nonZeros());
    for (int j = 0; j < 4; ++j) {
        CHECK(B.coeff(3 * j + 1, 3 * j + 0) == 1);
        CHECK(B.coeff(3 * j + 0, 3 * j + 1) == 2);
        CHECK(B.coeff(3 * j + 2, 3 * j + 2) == 3);
    }
}
