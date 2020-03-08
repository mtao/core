#include <iostream>
#include <mtao/geometry/mesh/stack_meshes.hpp>
#include <catch2/catch.hpp>
#include <iterator>
#include <iostream>
#include <algorithm>


TEST_CASE("2d_nocol", "[stack_meshes]") {
        std::vector<std::tuple<mtao::ColVecs2d,mtao::ColVecs3i>> tups;

        tups.emplace_back(
                mtao::ColVecs2d::Constant(2,4,0.0),
                mtao::ColVecs3i::Constant(2,1,0));
        tups.emplace_back(
                mtao::ColVecs2d::Constant(2,1,1.0),
                mtao::ColVecs3i::Constant(2,2,1));
        tups.emplace_back(
                mtao::ColVecs2d::Constant(2,3,2.0),
                mtao::ColVecs3i::Constant(2,3,2));

        auto [V,F] = mtao::geometry::mesh::stack_meshes(tups);

        int fm = F.maxCoeff();
        REQUIRE(V.cols() > fm);
        REQUIRE(V.cols() == 8);

        REQUIRE(F.cols() == 6);
        std::cout << "V:\n" << V << std::endl;
        std::cout << "F:\n" << F << std::endl;
}
TEST_CASE("2d_col", "[stack_meshes]") {
        std::vector<std::tuple<mtao::ColVecs2d,mtao::ColVecs3i, mtao::Vec4d>> tups;

        tups.emplace_back(
                mtao::ColVecs2d::Constant(2,4,0.0),
                mtao::ColVecs3i::Constant(2,1,0), mtao::Vec4d::Zero());
        tups.emplace_back(
                mtao::ColVecs2d::Constant(2,1,1.0),
                mtao::ColVecs3i::Constant(2,2,1), mtao::Vec4d::Ones());
        tups.emplace_back(
                mtao::ColVecs2d::Constant(2,3,2.0),
                mtao::ColVecs3i::Constant(2,3,2), mtao::Vec4d::Zero());

        auto [V,F,C] = mtao::geometry::mesh::stack_meshes(tups);
        int fm = F.maxCoeff();
        REQUIRE(4 == C.rows());
        REQUIRE(V.cols() == C.cols());
        REQUIRE(V.cols() > fm);
        REQUIRE(V.cols() == 8);

        REQUIRE(F.cols() == 6);
        std::cout << "V:\n" << V << std::endl;
        std::cout << "F:\n" << F << std::endl;
        std::cout << "C:\n" << C << std::endl;

}
TEST_CASE("2d__single_vel", "[stack_meshes]") {
        std::vector<mtao::ColVecs3i> tups;

        mtao::ColVecs2f VV(2,3);
        VV.col(0).setConstant(0);
        VV.col(1).setConstant(1);
        VV.col(2).setConstant(2);

        tups.emplace_back(
                mtao::ColVecs3i::Constant(2,1,0));
        tups.emplace_back(
                mtao::ColVecs3i::Constant(2,2,1));
        tups.emplace_back(
                mtao::ColVecs3i::Constant(2,3,2));

        auto [V,F] = mtao::geometry::mesh::stack_meshes(VV,tups);
        int fm = F.maxCoeff();
        REQUIRE(V.cols() > fm);
        REQUIRE(V.cols() == 3);

        REQUIRE(F.cols() == 6);
        std::cout << "V:\n" << V << std::endl;
        std::cout << "F:\n" << F << std::endl;

}
TEST_CASE("2d_col_single_vel", "[stack_meshes]") {
        std::vector<std::tuple<mtao::ColVecs3i, mtao::Vec4f>> tups;

        mtao::ColVecs2f VV(2,3);
        VV.col(0).setConstant(0);
        VV.col(1).setConstant(1);
        VV.col(2).setConstant(2);

        tups.emplace_back(
                mtao::ColVecs3i::Constant(2,1,0), mtao::Vec4f::Zero());
        tups.emplace_back(
                mtao::ColVecs3i::Constant(2,2,1), mtao::Vec4f::Ones());
        tups.emplace_back(
                mtao::ColVecs3i::Constant(2,3,2), mtao::Vec4f::Zero());

        auto [V,F,C] = mtao::geometry::mesh::stack_meshes(VV,tups);
        int fm = F.maxCoeff();
        REQUIRE(4 == C.rows());
        REQUIRE(V.cols() == C.cols());
        REQUIRE(V.cols() > fm);
        REQUIRE(V.cols() == 3);

        REQUIRE(F.cols() == 6);
        std::cout << "V:\n" << V << std::endl;
        std::cout << "F:\n" << F << std::endl;
        std::cout << "C:\n" << C << std::endl;

}
