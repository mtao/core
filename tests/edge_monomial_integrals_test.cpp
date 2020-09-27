#include <fmt/format.h>

#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/edge_monomial_integrals.hpp>

TEST_CASE("edge_monomials", "[trigonometry]") {
    mtao::Vec2d a(0., 0.);
    mtao::Vec2d b(1., 0.);
    mtao::Vec2d c(0., 1.);
    mtao::Vec2d d(1., 1.);

    const int N = 4;
    // auto integrals =
    // mtao::geometry::edge_monomial_integrals<double>(N+1);
    auto integralsx = mtao::geometry::edge_monomial_integrals<double>(N, a, b);
    auto integralsy = mtao::geometry::edge_monomial_integrals<double>(N, a, c);
    auto integralsd = mtao::geometry::edge_monomial_integrals<double>(N, a, d);

    fmt::print("{}\n", fmt::join(integralsx, " "));
    fmt::print("{}\n", fmt::join(integralsy, " "));
    fmt::print("{}\n", fmt::join(integralsd, " "));

    auto get_integralx = [&](size_t degree, size_t index) {
        size_t off = (degree * (degree + 1)) / 2;
        int k = off + index;
        return integralsx[k];
    };
    auto get_integraly = [&](size_t degree, size_t index) {
        size_t off = (degree * (degree + 1)) / 2;
        int k = off + index;
        return integralsy[k];
    };
    CHECK(get_integralx(0, 0) == Approx(1));

    CHECK(get_integralx(1, 0) == Approx(0.5));
    CHECK(get_integralx(1, 1) == Approx(0.));

    CHECK(get_integralx(2, 0) == Approx(1. / 3));
    CHECK(get_integralx(2, 1) == Approx(0.));
    CHECK(get_integralx(2, 2) == Approx(0.));

    CHECK(get_integralx(3, 0) == Approx(1. / 4));
    CHECK(get_integralx(3, 1) == Approx(0));
    CHECK(get_integralx(3, 2) == Approx(0));
    CHECK(get_integralx(3, 3) == Approx(0));

    CHECK(get_integralx(4, 0) == Approx(1. / 5));
    CHECK(get_integralx(4, 1) == Approx(0.));
    CHECK(get_integralx(4, 2) == Approx(0.));
    CHECK(get_integralx(4, 3) == Approx(0.));
    CHECK(get_integralx(4, 4) == Approx(0.));

    std::cout << std::endl;
    CHECK(get_integraly(0, 0) == Approx(1.));

    CHECK(get_integraly(1, 0) == Approx(0.));
    CHECK(get_integraly(1, 1) == Approx(.5));

    CHECK(get_integraly(2, 0) == Approx(0));
    CHECK(get_integraly(2, 1) == Approx(0));
    CHECK(get_integraly(2, 2) == Approx(1. / 3));

    CHECK(get_integraly(3, 0) == Approx(0));
    CHECK(get_integraly(3, 1) == Approx(0));
    CHECK(get_integraly(3, 2) == Approx(0));
    CHECK(get_integraly(3, 3) == Approx(1. / 4));

    CHECK(get_integraly(4, 0) == Approx(0));
    CHECK(get_integraly(4, 1) == Approx(0));
    CHECK(get_integraly(4, 2) == Approx(0));
    CHECK(get_integraly(4, 3) == Approx(0));
    CHECK(get_integraly(4, 4) == Approx(1. / 5));
}
