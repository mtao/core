#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/triangle_monomial_integrals.hpp>

TEST_CASE("triangle_monomials", "[trigonometry]") {
    mtao::Vec2d a(0., 0.);
    mtao::Vec2d b(1., 0.);
    mtao::Vec2d c(0., 1.);

    const int N = 4;
    auto integrals = mtao::geometry::triangle_monomial_integrals<double>(N);
    auto integrals2 =
        mtao::geometry::triangle_monomial_integrals<double>(N, a, b, c);

    for (auto&& v : integrals2) {
        std::cout << v << std::endl;
    }

    for (int k = 0; k < N; ++k) {
        int off = (k * (k + 1)) / 2;
        for (int j = 0; j < k + 1; ++j) {
            int index = off + j;
            std::cout << index << " / " << integrals.size() << ": ";
            std::cout << integrals[index] << " || ";
            std::cout << integrals2[index] << std::endl;
        }
        std::cout << std::endl;
    }
}
