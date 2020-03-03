#include <catch2/catch.hpp>
#include <mtao/eigen/sparse_blocks.hpp>


TEST_CASE("square square", "[merging]") {
    Eigen::SparseMatrix<double> A = Eigen::Matrix3d::Constant(0).sparseView();
    Eigen::SparseMatrix<double> B = Eigen::Matrix3d::Constant(1).sparseView();
    Eigen::SparseMatrix<double> C = Eigen::Matrix3d::Constant(2).sparseView();
    Eigen::SparseMatrix<double> D = Eigen::Matrix3d::Constant(3).sparseView();

    auto R = mtao::eigen::merge_sparse_blocks(A,B,C,D);
    for(int a = 0; a < 3; ++a) {
        for(int b = 0; b < 3; ++b) {
            REQUIRE(R.coeff(3*0+a,3*0+b) == 0);
            REQUIRE(R.coeff(3*0+a,3*1+b) == 1);
            REQUIRE(R.coeff(3*1+a,3*0+b) == 2);
            REQUIRE(R.coeff(3*1+a,3*1+b) == 3);
        }
    }


    auto Q = mtao::eigen::merge_sparse_blocks(A,B,C);
    for(int a = 0; a < 3; ++a) {
        for(int b = 0; b < 3; ++b) {
            REQUIRE(Q.coeff(3*0+a,3*0+b) == 0);
            REQUIRE(Q.coeff(3*0+a,3*1+b) == 2);
            REQUIRE(Q.coeff(3*1+a,3*0+b) == 2);
            REQUIRE(Q.coeff(3*1+a,3*1+b) == 1);
        }
    }
}
TEST_CASE("block merging rectangular", "[merging]") {
    {
        Eigen::SparseMatrix<double> A = Eigen::Matrix3d::Constant(0).sparseView();
        Eigen::SparseMatrix<double> B = Eigen::Matrix2d::Constant(1).sparseView();
        Eigen::SparseMatrix<double> C = Eigen::Matrix<double,3,2>::Constant(2).sparseView();
        Eigen::SparseMatrix<double> D = Eigen::Matrix<double,2,3>::Constant(3).sparseView();

        auto R = mtao::eigen::merge_sparse_blocks(A,C,D,B);
        for(int a = 0; a < A.rows(); ++a) {
            for(int b = 0; b < A.cols(); ++b) {
                REQUIRE(R.coeff(0 * A.rows() +a,0 * A.cols() +b) == 0);
            }
        }
        for(int a = 0; a < B.rows(); ++a) {
            for(int b = 0; b < B.cols(); ++b) {
                REQUIRE(R.coeff(1 * A.rows() +a,1 * A.cols() +b) == 1);
            }
        }
        for(int a = 0; a < C.rows(); ++a) {
            for(int b = 0; b < C.cols(); ++b) {
                REQUIRE(R.coeff(0 * A.rows() +a,1 * A.cols() +b) == 2);
            }
        }
        for(int a = 0; a < D.rows(); ++a) {
            for(int b = 0; b < D.cols(); ++b) {
                REQUIRE(R.coeff(1 * A.rows() +a,0 * A.cols() +b) == 3);
            }
        }
    }


    {
        Eigen::SparseMatrix<double> A = Eigen::Matrix3d::Constant(0).sparseView();
        Eigen::SparseMatrix<double> B = Eigen::Matrix2d::Constant(1).sparseView();
        Eigen::SparseMatrix<double> C = Eigen::Matrix<double,3,2>::Constant(2).sparseView();

        auto R = mtao::eigen::merge_sparse_blocks(A,B,C);
        for(int a = 0; a < A.rows(); ++a) {
            for(int b = 0; b < A.cols(); ++b) {
                REQUIRE(R.coeff(0 * A.rows() +a,0 * A.cols() +b) == 0);
            }
        }
        for(int a = 0; a < B.rows(); ++a) {
            for(int b = 0; b < B.cols(); ++b) {
                REQUIRE(R.coeff(1 * A.rows() +a,1 * A.cols() +b) == 1);
            }
        }
        for(int a = 0; a < C.rows(); ++a) {
            for(int b = 0; b < C.cols(); ++b) {
                REQUIRE(R.coeff(0 * A.rows() +a,1 * A.cols() +b) == 2);
                REQUIRE(R.coeff(1 * A.rows() +b,0 * A.cols() +b) == 2);
            }
        }
    }
}
