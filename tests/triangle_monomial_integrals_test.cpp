#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/triangle_monomial_integrals.hpp>
#include <fmt/format.h>

TEST_CASE("triangle_monomials", "[trigonometry]") {
    mtao::Vec2d a(0., 0.);
    mtao::Vec2d b(1., 0.);
    mtao::Vec2d c(0., 1.);

    const int N = 4;
    // auto integrals =
    // mtao::geometry::triangle_monomial_integrals<double>(N+1);
    auto integrals2 =
      mtao::geometry::triangle_monomial_integrals<double>(N, a, b, c);

    // for (auto&& v : integrals) {
    //    std::cout << v << " ";
    //}
    // std::cout << std::endl;
    // for (auto&& v : integrals2) {
    //    std::cout << v << " ";
    //}
    // std::cout << std::endl;

    for (auto &&v : integrals2) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    auto get_integral = [&](size_t degree, size_t index) {
        size_t off = (degree * (degree + 1)) / 2;
        int k = off + index;
        return integrals2[k];
    };
    CHECK(get_integral(0, 0) == Approx(1 * .5));

    CHECK(get_integral(1, 0) == Approx(1. / 6));
    CHECK(get_integral(1, 1) == Approx(1. / 6));

    CHECK(get_integral(2, 0) == Approx(1. / 12));
    CHECK(get_integral(2, 1) == Approx(1. / 24));
    CHECK(get_integral(2, 2) == Approx(1. / 12));

    CHECK(get_integral(3, 0) == Approx(1. / 20));
    CHECK(get_integral(3, 1) == Approx(1. / 60));
    CHECK(get_integral(3, 2) == Approx(1. / 60));
    CHECK(get_integral(3, 3) == Approx(1. / 20));

    CHECK(get_integral(4, 0) == Approx(1. / 30));
    CHECK(get_integral(4, 1) == Approx(1. / 120));
    CHECK(get_integral(4, 2) == Approx(1. / 180));
    CHECK(get_integral(4, 3) == Approx(1. / 120));
    CHECK(get_integral(4, 4) == Approx(1. / 30));

    a << 1.0, 0.0;
    b << 0.0, 0.0;
    c << 0.0, 1.0;
    integrals2 =
      mtao::geometry::triangle_monomial_integrals<double>(N, a, b, c);
    for (auto &&v : integrals2) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    CHECK(get_integral(0, 0) == Approx(-1 * .5));

    CHECK(get_integral(1, 0) == Approx(-1. / 6));
    CHECK(get_integral(1, 1) == Approx(-1. / 6));

    CHECK(get_integral(2, 0) == Approx(-1. / 12));
    CHECK(get_integral(2, 1) == Approx(-1. / 24));
    CHECK(get_integral(2, 2) == Approx(-1. / 12));

    CHECK(get_integral(3, 0) == Approx(-1. / 20));
    CHECK(get_integral(3, 1) == Approx(-1. / 60));
    CHECK(get_integral(3, 2) == Approx(-1. / 60));
    CHECK(get_integral(3, 3) == Approx(-1. / 20));

    CHECK(get_integral(4, 0) == Approx(-1. / 30));
    CHECK(get_integral(4, 1) == Approx(-1. / 120));
    CHECK(get_integral(4, 2) == Approx(-1. / 180));
    CHECK(get_integral(4, 3) == Approx(-1. / 120));
    CHECK(get_integral(4, 4) == Approx(-1. / 30));

    a << 0.0, 0.0;
    b << 2.0, 0.0;
    c << 0.0, 2.0;
    integrals2 =
      mtao::geometry::triangle_monomial_integrals<double>(N, a, b, c);
    CHECK(get_integral(0, 0) == Approx(4 * .5));

    CHECK(get_integral(1, 0) == Approx(8. / 6));
    CHECK(get_integral(1, 1) == Approx(8. / 6));

    CHECK(get_integral(2, 0) == Approx(16. / 12));
    CHECK(get_integral(2, 1) == Approx(16. / 24));
    CHECK(get_integral(2, 2) == Approx(16. / 12));

    CHECK(get_integral(3, 0) == Approx(32. / 20));
    CHECK(get_integral(3, 1) == Approx(32. / 60));
    CHECK(get_integral(3, 2) == Approx(32. / 60));
    CHECK(get_integral(3, 3) == Approx(32. / 20));

    CHECK(get_integral(4, 0) == Approx(64. / 30));
    CHECK(get_integral(4, 1) == Approx(64. / 120));
    CHECK(get_integral(4, 2) == Approx(64. / 180));
    CHECK(get_integral(4, 3) == Approx(64. / 120));
    CHECK(get_integral(4, 4) == Approx(64. / 30));
    // a << 1.0,1.0;
    // b << 3.0,1.0;
    // c << 1.0,3.0;
    // integrals2 =
    //    mtao::geometry::triangle_monomial_integrals<double>(N, a, b, c);

    // for (int level = 0; level <= N; ++level) {
    //    int off = (level * (level + 1)) / 2;
    //    for (int j = 0; j < level + 1; ++j) {
    //        int k = level - j;

    //        double val = 0.0;
    //        size_t samples = 0;
    //        for(double s = 0.0; s < 1.0; s+=.01) {
    //            for(double t = 0.0; t < 1.0 - s; t+=.01) {
    //                auto v = a + s * (b-a) + (c-a) * t;
    //                val += std::pow<double>(v.x(),j) *
    //                std::pow<double>(v.y(),k); samples++;
    //            }
    //        }
    //        val /= samples;
    //        val *= .5 * (b-a).homogeneous().cross((c-a).homogeneous()).z();

    //        std::cout << val << ": ";
    //        std::cout << get_integral(level,j) << std::endl;
    //    }
    //    std::cout << std::endl;
    //}
}
TEST_CASE("triangle_monomials3", "[trigonometry]") {
    mtao::Vec3d a(0., 0., 0.0);
    mtao::Vec3d b(1., 0., 0.0);
    mtao::Vec3d c(0., 1., 0.0);

    const int N = 2;
    // auto integrals =
    // mtao::geometry::triangle_monomial_integrals<double>(N+1);
    auto integrals2 =
      mtao::geometry::triangle_monomial_integrals<double>(N, a, b, c);

    // for (auto&& v : integrals) {
    //    std::cout << v << " ";
    //}
    // std::cout << std::endl;
    // for (auto&& v : integrals2) {
    //    std::cout << v << " ";
    //}
    // std::cout << std::endl;

    for (auto &&v : integrals2) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    auto get_integral = [&](size_t degree, size_t index) {
        size_t off = (degree * (degree + 1)) / 2;
        int k = off + index;
        return integrals2[k];
    };
    CHECK(get_integral(0, 0) == Approx(1 * .5));

    CHECK(get_integral(1, 0) == Approx(1. / 6));
    CHECK(get_integral(1, 1) == Approx(1. / 6));
    CHECK(get_integral(1, 2) == Approx(1. / 2));// constant function

    // CHECK(get_integral(2, 0) == Approx(1. / 12));
    // CHECK(get_integral(2, 1) == Approx(1. / 24));
    // CHECK(get_integral(2, 2) == Approx(1. / 12));
    // CHECK(get_integral(2, 3) == Approx(1. / 12));
    // CHECK(get_integral(2, 4) == Approx(1. / 12));
    // CHECK(get_integral(2, 5) == Approx(1. / 12));

    // CHECK(get_integral(3, 0) == Approx(1. / 20));
    // CHECK(get_integral(3, 1) == Approx(1. / 60));
    // CHECK(get_integral(3, 2) == Approx(1. / 60));
    // CHECK(get_integral(3, 3) == Approx(1. / 20));

    // CHECK(get_integral(4, 0) == Approx(1. / 30));
    // CHECK(get_integral(4, 1) == Approx(1. / 120));
    // CHECK(get_integral(4, 2) == Approx(1. / 180));
    // CHECK(get_integral(4, 3) == Approx(1. / 120));
    // CHECK(get_integral(4, 4) == Approx(1. / 30));
    //for (int level = 0; level <= N; ++level) {
    //    fmt::print("Level {}\n", level);
    //    int off = (level * (level + 1)) / 2;
    //   for (int j = 0; j < level + 1; ++j) {
    //       for(int k = 0; k <= level - j; ++k) {
    //           fmt::print("j = {}, k = {}\n", j,k);

    //           double val = 0.0;
    //           size_t samples = 0;
    //           for(double s = 0.0; s < 1.0; s+=.01) {
    //               for(double t = 0.0; t < 1.0 - s; t+=.01) {
    //                   auto v = a + s * (b-a) + (c-a) * t;
    //                   val += std::pow<double>(v.x(),j) *
    //                       std::pow<double>(v.y(),k)*
    //                       std::pow<double>(v.z(),level-j-k); samples++;
    //               }
    //           }
    //           val /= samples;
    //           val *= .5 * (b-a).cross(c-a).norm();

    //           std::cout << val << ": ";
    //           std::cout << get_integral(level,j+k) << std::endl;
    //       }
    //       std::cout << std::endl;
    //   }
    //   std::cout << std::endl;
    //}
}
