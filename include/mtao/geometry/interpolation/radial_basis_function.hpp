#pragma once
#include <cmath>

#include "mtao/eigen/shape_checks.hpp"
#include "mtao/types.hpp"

namespace mtao::geometry::interpolation {

// The common flag "make_pou" detrmines whether a RBF should be assumed to be a
// partition of unity

template <typename RBFunc, typename PointsType, typename VecType>
auto radial_basis_function(RBFunc&& f, const Eigen::MatrixBase<PointsType>& P,
                           const Eigen::MatrixBase<VecType>& v,
                           typename PointsType::Scalar radius,
                           bool make_pou = false)
    -> mtao::VectorX<typename PointsType::Scalar> {
    static_assert(
        std::is_same_v<typename PointsType::Scalar, typename VecType::Scalar>);
    eigen::col_check_with_assert<1>(v);

    using Scalar = typename PointsType::Scalar;
    mtao::VectorX<Scalar> R(P.cols());
    mtao::RowVectorX<Scalar> D = (P.colwise() - v).colwise().norm() / radius;

    R = D.unaryExpr(f).transpose();  // / radius;
    if (make_pou) {
        R /= R.sum();
    }
    return R;
}
template <typename PointsType, typename VecType>
auto gaussian_rbf(const Eigen::MatrixBase<PointsType>& P,
                  const Eigen::MatrixBase<VecType>& v,
                  typename PointsType::Scalar radius, bool make_pou = false) {
    using Scalar = typename PointsType::Scalar;
    radius /= 2;
    auto R = radial_basis_function(
        [](Scalar v) -> Scalar { return std::exp(-.5 * v * v); }, P, v, radius,
        make_pou);
    if (!make_pou) {
        R /= (std::sqrt(2 * M_PI) * radius);
    }
    return R;
}

template <typename PointsType, typename VecType>
auto spline_gaussian_rbf(const Eigen::MatrixBase<PointsType>& P,
                         const Eigen::MatrixBase<VecType>& v,
                         typename PointsType::Scalar radius,
                         bool make_pou = false) {
    using Scalar = typename PointsType::Scalar;
    radius /= 2;
    auto R = radial_basis_function(
        [](Scalar v) -> Scalar {
            if (v < 1) {
                return 1 - 1.5 * v * v + .75 * v * v * v;
            } else if (v < 2) {
                Scalar t = 2 - v;
                return .25 * (t * t * t);
            }
            return 0;
        },
        P, v, radius, make_pou);
    if (!make_pou) {
        R /= (M_PI * radius);
    }
    return R;
}
template <typename PointsType, typename VecType>
auto desbrun_spline_rbf(const Eigen::MatrixBase<PointsType>& P,
                        const Eigen::MatrixBase<VecType>& v,
                        typename PointsType::Scalar radius,
                        bool make_pou = false) {
    using Scalar = typename PointsType::Scalar;
    radius /= 2;
    auto R = radial_basis_function(
        [](Scalar v) -> Scalar {
            if (v < 2) {
                Scalar t = 2 - v;
                return t * t * t;
            }
            return 0;
        },
        P, v, radius, make_pou);
    if (!make_pou) {
        R *= 15;
        R /= (M_PI * radius * radius * radius * 64);
    }
    return R;
}

template <typename RBGrad, typename PointsType, typename VecType>
auto radial_basis_gradient(RBGrad&& g, const Eigen::MatrixBase<PointsType>& P,
                           const Eigen::MatrixBase<VecType>& v,
                           typename PointsType::Scalar radius)
    -> mtao::VectorX<typename PointsType::Scalar> {
    static_assert(
        std::is_same_v<typename PointsType::Scalar, typename VecType::Scalar>);
    eigen::col_check_with_assert<1>(v);

    using Scalar = typename PointsType::Scalar;
    auto R = (P.colwise() - v).eval();
    RowVectorX<Scalar> D = R.colwise().norm();
    R.array().rowwise() /= D.array();

    R.array().rowwise() *= D.unaryExpr(g).array();
    return R;
}
template <typename PointsType, typename VecType>
auto gaussian_rbg(const Eigen::MatrixBase<PointsType>& P,
                  const Eigen::MatrixBase<VecType>& v,
                  typename PointsType::Scalar radius) {
    using Scalar = typename PointsType::Scalar;
    auto R = radial_basis_function(
        [](Scalar v) -> Scalar { return -v * std::exp(-.5 * v * v); }, P, v,
        radius);
    R /= (std::sqrt(2 * M_PI) * radius);
    return R;
}

template <typename PointsType, typename VecType>
auto spline_gaussian_rbg(const Eigen::MatrixBase<PointsType>& P,
                         const Eigen::MatrixBase<VecType>& v,
                         typename PointsType::Scalar radius) {
    using Scalar = typename PointsType::Scalar;
    auto R = radial_basis_function(
        [](Scalar v) -> Scalar {
            if (v < 1) {
                return 1 - 3 * v + 2.25 * v * v;
            } else if (v < 2) {
                Scalar t = 2 - v;
                return .75 * (t * t) * (-v);
            }
            return 0;
        },
        P, v, radius);
    R /= (M_PI * radius);
    return R;
}
template <typename PointsType, typename VecType>
auto desbrun_spline_rbg(const Eigen::MatrixBase<PointsType>& P,
                        const Eigen::MatrixBase<VecType>& v,
                        typename PointsType::Scalar radius) {
    using Scalar = typename PointsType::Scalar;
    auto R = radial_basis_function(
        [](Scalar v) -> Scalar {
            if (v < 2) {
                Scalar t = 2 - v;
                return 3 * t * t * (-v);
            }
            return 0;
        },
        P, v, radius);
    R *= 15;
    R /= (M_PI * radius * radius * radius * 64);
    return R;
}
}  // namespace mtao::geometry::interpolation
