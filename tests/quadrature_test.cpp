#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/quadrature/simpsons.hpp>

using namespace mtao::quadrature;

TEST_CASE("simpsons", "[quadrature]") {
    std::cout << simpsons_rule(
                     [](double x) -> double { return std::pow(x, 5); }, 0.,
                     double(1), /*samples=*/10)
              << std::endl;
    std::cout << simpsons_rule(
                     [](double x) -> double { return std::pow(x, 3); }, 0.,
                     double(1), /*samples=*/15)
              << std::endl;
    std::cout << simpsons_rule(
                     [](double x) -> double { return std::pow(x, 4); }, 0.,
                     double(1), /*samples=*/10)
              << std::endl;
    for (int j = 0; j < 10; ++j) {
        for (int k = 0; k < 4; ++k) {
            double v = simpsons_rule(
                [k](double x) -> double { return std::pow(x, k); }, 0.,
                double(j), /*samples=*/k);
            double a = 1 / (k + 1.) * std::pow<double>(j, k + 1);
            CHECK(v == Approx(a));
        }
    }

    CHECK(multidim_simpsons_rule<2>(
              [](const std::array<double, 2>& x) -> double {
                  return std::pow(x[0], 5);
              },
              0., double(1), /*samples=*/30) == Approx(1 / 6.));

    CHECK(multidim_simpsons_rule<2>(
              [](const std::array<double, 2>& x) -> double {
                  return std::pow(x[1], 5);
              },
              0., double(1), /*samples=*/30) == Approx(1 / 6.));
    CHECK(multidim_simpsons_rule<3>(
              [](const std::array<double, 3>& x) -> double {
                  return std::pow(x[0], 5);
              },
              0., double(1), /*samples=*/30) == Approx(1 / 6.));

    CHECK(multidim_simpsons_rule<3>(
              [](const std::array<double, 3>& x) -> double {
                  return std::pow(x[1], 5);
              },
              0., double(1), /*samples=*/30) == Approx(1 / 6.));
    CHECK(multidim_simpsons_rule<2>(
              [](const std::array<double, 2>& x) -> double {
                  return std::pow(x[0] * x[1], 5);
              },
              0., double(1), /*samples=*/30) == Approx(1 / 36.));
}
