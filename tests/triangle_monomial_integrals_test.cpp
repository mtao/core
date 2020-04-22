#include <iostream>
#include <mtao/geometry/triangle_monomial_integrals.hpp>
#include <catch2/catch.hpp>


TEST_CASE("triangle_monomials", "[trigonometry]") {
    const int N = 4;
    auto integrals = mtao::geometry::triangle_monomial_integrals<double>(N);

    for(int k = 0; k < N; ++k) {
        int off = (k*(k+1)) / 2;
        for(int j = 0; j < k+1; ++j) {
            int index = off + j;
            std::cout << index << " / " << integrals.size() << ": ";
            std::cout << integrals[index] << std::endl;
        }
        std::cout << std::endl;
    }

}
