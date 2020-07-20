#pragma once
namespace mtao::quadrature {
// returns teh weights and points of a gauss lobatto rule
// https://en.wikipedia.org/wiki/Gaussian_quadrature#Gauss%E2%80%93Lobatto_rules
template <typename T>
std::tuple<std::vector<T>, std::vector<T>> gauss_lobatto_(int n = 3) const {
    const static T s5 = T(1) / std::sqrt<T>(5);
    const static T s37 = std::sqrt<T>(T(3) / 7);
    const static std::vector<std::vector<T>> points{
        {T(-1), T(0), T(1)},
        {T(-1), -s5, s5, T(1)},
        {T(-1), -s37, T(0), s37, T(1)}};
    const static std::vector<std::vector<T>> weights{
        {T(1) / 3, T(4) / 3, T(1) / 3},
        {T(1) / 6, T(5) / 6, T(5) / 6, T(1) / 6},
        {T(1) / 10, T(49) / 90, T(32) / 45, T(49) / 90, T(1) / 10}};

    return {points.at(n - 3), weights.at(n - 3)};
};
}  // namespace mtao::quadrature
}

}  // namespace mtao::quadrature
