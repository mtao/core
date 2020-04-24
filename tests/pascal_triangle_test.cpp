#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/algebra/combinatorial.hpp>
#include <mtao/algebra/pascal_triangle.hpp>

TEST_CASE("PascalTriangle", "[algebra]") {
    for (size_t l = 0; l < 10; ++l) {
        mtao::algebra::PascalTriangle pt(l);
        REQUIRE(pt.entries().size() == ((l + 1) * (l + 2)) / 2);

        for (size_t j = 0; j <= pt.levels(); ++j) {
            for (size_t k = 0; k < j + 1; ++k) {
                CHECK(pt(j, k) == mtao::combinatorial::nCr(j, k));
                // std::cout << pt(j, k) << " ";
            }
            // std::cout << std::endl;
        }
    }
}
