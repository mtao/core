#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/eigen/diagonal_to_sparse.hpp>
#include <mtao/types.hpp>

TEST_CASE("Vector to Diag", "[diagonal,sparse]") {
    mtao::VecXd x(5);
    x << 1, 2, 3, 4, 5;

    {
        auto A = mtao::eigen::diagonal_to_sparse(x);

        CHECK(A.coeff(0, 0) == 1);
        CHECK(A.coeff(1, 1) == 2);
        CHECK(A.coeff(2, 2) == 3);
        CHECK(A.coeff(3, 3) == 4);
        CHECK(A.coeff(4, 4) == 5);
    }

    {
        auto A = mtao::eigen::diagonal_to_sparse(x.asDiagonal());

        CHECK(A.coeff(0, 0) == 1);
        CHECK(A.coeff(1, 1) == 2);
        CHECK(A.coeff(2, 2) == 3);
        CHECK(A.coeff(3, 3) == 4);
        CHECK(A.coeff(4, 4) == 5);
    }
    {
        auto A = mtao::eigen::diagonal_to_sparse(3 * x.asDiagonal());

        CHECK(A.coeff(0, 0) == 3);
        CHECK(A.coeff(1, 1) == 6);
        CHECK(A.coeff(2, 2) == 9);
        CHECK(A.coeff(3, 3) == 12);
        CHECK(A.coeff(4, 4) == 15);
    }
    {
        auto A =
          mtao::eigen::diagonal_to_sparse(Eigen::MatrixXd::Identity(5, 5));

        CHECK(A.coeff(0, 0) == 1);
        CHECK(A.coeff(1, 1) == 1);
        CHECK(A.coeff(2, 2) == 1);
        CHECK(A.coeff(3, 3) == 1);
        CHECK(A.coeff(4, 4) == 1);
    }
    {
        auto A =
          mtao::eigen::diagonal_to_sparse(3 * Eigen::Matrix3d::Identity());

        CHECK(A.coeff(0, 0) == 3);
        CHECK(A.coeff(1, 1) == 3);
        CHECK(A.coeff(2, 2) == 3);
    }
}
