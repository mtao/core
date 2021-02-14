#include <mtao/geometry/solid_angle.hpp>
#include <mtao/types.hpp>
#include <iostream>
#include <catch2/catch.hpp>
#include <mtao/geometry/mesh/shapes/sphere.hpp>

TEST_CASE("solid angles", "[trigonometry]") {
    using namespace mtao::geometry;

    auto [V, F] = mesh::shapes::sphere<double>(3);


    CHECK(solid_angle_mesh(V, F, mtao::Vec3d::Zero()) == Approx(1.0));
    CHECK(solid_angle_mesh(V, F, mtao::Vec3d::Constant(2.0)) == Approx(0.0).margin(1e-10));


    mtao::Mat3d A = mtao::Mat3d::Identity();

    std::cout << solid_angle(A) << " " << solid_angle(A.col(0), A.col(1), A.col(2)) << std::endl;
    for (int i = 0; i < 5; ++i) {
        auto v = mtao::Vec3d::Random().eval();
        CHECK(solid_angle(A, v) == Approx(solid_angle(A.col(0), A.col(1), A.col(2), v)));
    }
}
