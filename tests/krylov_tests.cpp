#include <iostream>
#include <mtao/linear_algebra/arnoldi.hpp>
#include <mtao/linear_algebra/lanczos.hpp>
#include <catch2/catch.hpp>
#include <iterator>
#include <iostream>
#include <algorithm>


template<typename Derived, typename Derived2>
void test_mats(const Eigen::MatrixBase<Derived> &A, const Eigen::MatrixBase<Derived2> &B) {
    REQUIRE(A.rows() == B.rows());
    REQUIRE(A.cols() == B.cols());
    if (A.rows() == B.rows() && A.cols() == B.cols()) {
        for (int i = 0; i < A.rows(); ++i) {
            for (int j = 0; j < B.cols(); ++j) {
                REQUIRE(A(i, j) == Approx(B(i, j)).margin(1e-5));
            }
        }
    }
}


TEST_CASE("Arnoldi", "[arnoldi]") {

    for (auto &&D : { 1, 2, 5, 10 }) {
        Eigen::MatrixXd L(D, D);
        L.setRandom();

        Eigen::VectorXd B(D);
        B.setRandom();
        B.normalize();

        auto [Q, H] = mtao::linear_algebra::arnoldi(L, B, D);
        test_mats(L, Q * H * Q.transpose());
    }
}
TEST_CASE("Symmetric", "[Lanscoz,arnoldi]") {

    for (auto &&D : { 1, 2, 5, 10 }) {
        Eigen::MatrixXd L(D, D);
        L.setRandom();
        L = L * L.transpose();

        Eigen::VectorXd B(D);
        B.setRandom();
        B.normalize();

        auto [Q, H] = mtao::linear_algebra::arnoldi(L, B, D);
        auto [V, T] = mtao::linear_algebra::lanczos(L, B, D);
        test_mats(Q, V);
        test_mats(H, T);
        test_mats(L, Q * H * Q.transpose());
        test_mats(L, V * T * V.transpose());
    }
}
