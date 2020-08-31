#pragma once
#include "mtao/iterator/zip.hpp"
#include "mtao/eigen/stl2eigen.hpp"
#include <vector>
namespace mtao::quadrature {
// returns teh weights and points of a gauss lobatto rule on the [-1,1] interval
// https://en.wikipedia.org/wiki/Gaussian_quadrature#Gauss%E2%80%93Lobatto_rules
template <typename T>
std::tuple<const std::vector<T>&,
           const std::vector<T>&>
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

    return {points.at(n - 3), weights.at(n - 3)};
}

template <typename Scalar>
mtao::VectorX<Scalar>
gauss_lobatto_sample_points(int n, Scalar min, Scalar max) {
    auto&& [P, W] = gauss_lobatto_data<Scalar>(n);
    Scalar mid = (min + max) / 2;
    Scalar halfrange = (max - min) / 2;
    return mtao::eigen::stl2eigen(P).array() * halfrange + mid;
}
template <typename Scalar>
Scalar
gauss_lobatto_sample_point(int n, Scalar min, Scalar max, size_t position) {
    auto&& [P, W] = gauss_lobatto_data<Scalar>(n);
    Scalar mid = (min + max) / 2;
    Scalar halfrange = (max - min) / 2;
    return P.at(position) * halfrange + mid;
}

template <typename Scalar>
const auto&
gauss_lobatto_sample_weights(int n) {
    auto&& [P, W] = gauss_lobatto_data<Scalar>(n);
    return W;
}
template <typename Scalar>
Scalar
gauss_lobatto_sample_weight(int n, size_t position) {
    auto&& [P, W] = gauss_lobatto_data<Scalar>(n);
    return W.at(position);
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

template <typename Derived>
typename Derived::Scalar gauss_lobatto(const Eigen::MatrixBase<Derived>& f, typename Derived::Scalar range) {
    auto [P, W] = gauss_lobatto_data<typename Derived::Scalar>(f.rows());

    auto wm = mtao::eigen::stl2eigen(W);
    return f.dot(wm) * range / 2.;
}
template <typename Scalar>
Scalar gauss_lobatto(const std::vector<Scalar>& f, Scalar range) {

    return gauss_lobatto(mtao::eigen::stl2eigen(f),range);
}

}  // namespace mtao::quadrature
