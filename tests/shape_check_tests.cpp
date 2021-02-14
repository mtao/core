#include <mtao/eigen/shape_checks.hpp>
#include <mtao/eigen/concepts.hpp>


#include <iostream>
#include <catch2/catch.hpp>

using namespace mtao::eigen;

TEST_CASE("shape_checks_no_throws", "[shape_checks]") {

    CHECK(col_check<3>(Eigen::Matrix3d()));
    CHECK(row_check<3>(Eigen::Matrix3d()));
    CHECK(row_check<3>(Eigen::MatrixXd(3, 3)));
    CHECK(col_check<3>(Eigen::MatrixXd(3, 3)));
    // not sure how to check for a compilation failure like this...
    //CHECK(!col_check<3>(Eigen::Matrix2d()));
    //CHECK(!row_check<3>(Eigen::Matrix2d()));
    CHECK(!row_check<3>(Eigen::MatrixXd(2, 3)));
    CHECK(!col_check<3>(Eigen::MatrixXd(3, 2)));
}

TEST_CASE("shape_checks_throws", "[shape_checks]") {

    CHECK_NOTHROW(col_check_with_throw<3>(Eigen::Matrix3d()));
    CHECK_NOTHROW(row_check_with_throw<3>(Eigen::Matrix3d()));
    CHECK_NOTHROW(row_check_with_throw<3>(Eigen::MatrixXd(3, 3)));
    CHECK_NOTHROW(col_check_with_throw<3>(Eigen::MatrixXd(3, 3)));
    //CHECK_THROWS(col_check_with_throw<3>(Eigen::Matrix2d()));
    //CHECK_THROWS(row_check_with_throw<3>(Eigen::Matrix2d()));
    CHECK_THROWS(row_check_with_throw<3>(Eigen::MatrixXd(2, 3)));
    CHECK_THROWS(col_check_with_throw<3>(Eigen::MatrixXd(3, 2)));
}
