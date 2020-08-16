#pragma once
#include "mtao/iterator/zip.hpp"
namespace mtao::quadrature {
// returns teh weights and points of a gauss lobatto rule on the [-1,1] interval
// https://en.wikipedia.org/wiki/Gaussian_quadrature#Gauss%E2%80%93Lobatto_rules
template <typename T>
std::tuple<std::reference_wrapper<const std::vector<T>>,
           std::reference_wrapper<const std::vector<T>>>
gauss_lobatto_data(int n = 3) {
    const static T s5 = T(1) / std::sqrt<T>(5).real();
    const static T s37 = std::sqrt<T>(T(3) / 7).real();
    const static std::vector<std::vector<T>> points{
        {T(-1), T(0), T(1)},
        {T(-1), -s5, s5, T(1)},
        {T(-1), -s37, T(0), s37, T(1)}};
    const static std::vector<std::vector<T>> weights{
        {T(1) / 3, T(4) / 3, T(1) / 3},
        {T(1) / 6, T(5) / 6, T(5) / 6, T(1) / 6},
        {T(1) / 10, T(49) / 90, T(32) / 45, T(49) / 90, T(1) / 10}};

    return {std::cref(points.at(n - 3)), std::cref(weights.at(n - 3))};
}

template <typename Scalar, typename Func>
auto gauss_lobatto(Func&& f, int n, Scalar ret = 0) {
    auto&& [P, W] = gauss_lobatto_data<Scalar>(n);
    for (auto&& [p, w] : mtao::iterator::zip(P, W)) {
        ret += f(p) * w;
    }
    return ret;
}
template <typename Scalar, typename Func>
auto gauss_lobatto(Func&& f, Scalar min, Scalar max, int n, Scalar ret = 0) {
    auto [P, W] = gauss_lobatto_data<Scalar>(n);

    // mx + b
    // -m + b = min
    // m + b = max
    Scalar mid = (min + max) / 2;
    Scalar halfrange = (max - min) / 2;

    for (auto&& [p, w] : mtao::iterator::zip(P, W)) {
        ret += f(p * halfrange + mid) * w;
    }
    // invert the jacobian
    return halfrange * ret;
}

}  // namespace mtao::quadrature
