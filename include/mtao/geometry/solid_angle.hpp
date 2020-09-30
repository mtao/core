#pragma once
#include <Eigen/Core>
#include <cmath>
#include <iostream>
#include <type_traits>

#include "mtao/eigen/shape_checks.hpp"

namespace mtao::geometry {

// The Solid Angle of a Plane Triangle
// A. Van Oosterom, J. Strackee
// divided by 2*M_PI to make the solid angle from the interior of a closed
// geometry 1

// The actual implementations
template <typename AType, typename BType, typename CType>
auto solid_angle(const Eigen::MatrixBase<AType>& a,
                 const Eigen::MatrixBase<BType>& b,
                 const Eigen::MatrixBase<CType>& c) -> typename AType::Scalar {
    if (eigen::shape_check<3, 1>(a) && eigen::shape_check<3, 1>(b) &&
        eigen::shape_check<3, 1>(c)) {
        auto num = a.cross(b).dot(c);
        auto aN = a.norm();
        auto bN = b.norm();
        auto cN = c.norm();
        return std::atan2<typename AType::Scalar>(
                   num, aN * bN * cN + aN * b.dot(c) + bN * a.dot(c) +
                            cN * a.dot(b)) /
               (2 * M_PI);

    } else {
        assert(false);
        return 0;
    }
}
template <typename MType>
auto solid_angle(const Eigen::MatrixBase<MType>& M) -> typename MType::Scalar {
    if (eigen::shape_check<3, 3>(M)) {
        auto num = M.determinant();
        auto B = M.transpose() * M;
        auto C = B.diagonal().cwiseSqrt().eval();
        return std::atan2<typename MType::Scalar>(
                   num, C.prod() + C(0) * B(1, 2) + C(1) * B(0, 2) +
                            C(2) * B(0, 1)) /
               (2 * M_PI);

    } else {
        assert(false);
        return 0;
    }
}

// NOTE: this is going to be pretty slow for large meshes...
template <typename VType, typename FType>
auto solid_angle_mesh(const Eigen::MatrixBase<VType>& V,
                      const Eigen::MatrixBase<FType>& F) {
    static_assert(std::is_integral_v<typename FType::Scalar>);
    eigen::row_check_with_assert<3>(V);
    eigen::row_check_with_assert<3>(F);
    typename VType::Scalar ret = 0;
    for (int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        ret += solid_angle(V.col(f(0)), V.col(f(1)), V.col(f(2)));
    }
    return ret;
}

// Some convenience versions for when passing in a point to check
template <typename VType, typename FType, typename PType>
auto solid_angle_mesh(const Eigen::MatrixBase<VType>& V,
                      const Eigen::MatrixBase<FType>& F,
                      const Eigen::MatrixBase<PType>& p) {
    return solid_angle_mesh((V.colwise() - p).eval(), F);
}

template <typename AType, typename BType, typename CType, typename PType>
auto solid_angle(const Eigen::MatrixBase<AType>& a,
                 const Eigen::MatrixBase<BType>& b,
                 const Eigen::MatrixBase<CType>& c,
                 const Eigen::MatrixBase<PType>& p) {
    return solid_angle(a - p, b - p, c - p);
}

template <typename MType, typename PType>
auto solid_angle(const Eigen::MatrixBase<MType>& M,
                 const Eigen::MatrixBase<PType>& p) {
    static_assert(std::is_floating_point_v<typename PType::Scalar>);
    return solid_angle((M.colwise() - p).eval());
}
}  // namespace mtao::geometry
