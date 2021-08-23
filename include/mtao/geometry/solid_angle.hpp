#pragma once
#include <Eigen/Core>
#include <cmath>
#include <numbers>
#include <iostream>
#include <type_traits>

#include "mtao/eigen/shape_checks.hpp"

namespace mtao::geometry {

// The Solid Angle of a Plane Triangle
// A. Van Oosterom, J. Strackee
// divided by 2*pi_v to make the solid angle from the interior of a closed
// geometry 1

// The actual implementations
template<
  eigen::concepts::Vec3Compatible AType,
  eigen::concepts::Vec3Compatible BType,
  eigen::concepts::Vec3Compatible CType>


auto solid_angle(const AType &a,
                 const BType &b,
                 const CType &c) -> typename AType::Scalar {
    eigen::shape_check_with_throw<3, 1>(a);
    eigen::shape_check_with_throw<3, 1>(b);
    eigen::shape_check_with_throw<3, 1>(c);
    auto num = a.cross(b).dot(c);
    auto aN = a.norm();
    auto bN = b.norm();
    auto cN = c.norm();
    return std::atan2<typename AType::Scalar>(
             num, aN * bN * cN + aN * b.dot(c) + bN * a.dot(c) + cN * a.dot(b))
           / (2 * std::numbers::pi_v<typename AType::Scalar>);
}

template<eigen::concepts::SquareMatrix3Compatible MType>
auto solid_angle(const MType &M) -> typename MType::Scalar {

    eigen::shape_check_with_throw<3, 3>(M);
    auto num = M.determinant();
    auto B = M.transpose() * M;
    auto C = B.diagonal().cwiseSqrt().eval();
    return std::atan2<typename MType::Scalar>(
             num, C.prod() + C(0) * B(1, 2) + C(1) * B(0, 2) + C(2) * B(0, 1))
           / (2 * std::numbers::pi_v<typename MType::Scalar>);
}

// NOTE: this is going to be pretty slow for large meshes...
template<eigen::concepts::ColVecs3Compatible VType,
         eigen::concepts::ColVecs3Compatible FType>
auto solid_angle_mesh(const VType &V,
                      const FType &F) {
    static_assert(std::is_integral_v<typename FType::Scalar>);
    eigen::row_check_with_throw<3>(V);
    eigen::row_check_with_throw<3>(F);
    typename VType::Scalar ret = 0;
    for (int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        ret += solid_angle(V.col(f(0)), V.col(f(1)), V.col(f(2)));
    }
    return ret;
}

// Some convenience versions for when passing in a point to check
template<eigen::concepts::ColVecs3Compatible VType,
         eigen::concepts::ColVecs3Compatible FType,
         eigen::concepts::Vec3Compatible PType>
auto solid_angle_mesh(const VType &V,
                      const FType &F,
                      const PType &p) {
    eigen::row_check_with_throw<3>(V);
    eigen::row_check_with_throw<3>(F);
    eigen::shape_check_with_throw<3, 1>(p);
    return solid_angle_mesh((V.colwise() - p).eval(), F);
}

template<
  eigen::concepts::Vec3Compatible AType,
  eigen::concepts::Vec3Compatible BType,
  eigen::concepts::Vec3Compatible CType,
  eigen::concepts::Vec3Compatible PType>
auto solid_angle(const AType &a,
                 const BType &b,
                 const CType &c,
                 const PType &p) {
    eigen::shape_check_with_throw<3, 1>(a);
    eigen::shape_check_with_throw<3, 1>(b);
    eigen::shape_check_with_throw<3, 1>(c);
    eigen::shape_check_with_throw<3, 1>(p);
    return solid_angle(a - p, b - p, c - p);
}

template<eigen::concepts::SquareMatrix3Compatible MType, eigen::concepts::ColVecs3Compatible PType>
auto solid_angle(const MType &M,
                 const PType &p) {
    static_assert(std::is_floating_point_v<typename PType::Scalar>);

    eigen::shape_check_with_throw<3, 3>(M);
    eigen::shape_check_with_throw<3, 1>(p);
    return solid_angle((M.colwise() - p).eval());
}
}// namespace mtao::geometry
