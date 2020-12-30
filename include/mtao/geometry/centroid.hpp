#ifndef CENTROID_HPP
#define CENTROID_HPP
#include <Eigen/Core>
#include <cassert>
#include <numeric>

#include "mtao/eigen/shape_checks.hpp"
#include "mtao/geometry/volume.hpp"
#include "mtao/types.hpp"
#include "mtao/util.h"

namespace mtao::geometry {
/// \brief a 2D centroid evaluator for boundary curves
/// \param V A ColVector of 2d or points
template<typename Derived, typename BeginIt, typename EndIt>
auto curve_centroid(const Eigen::MatrixBase<Derived> &V, const BeginIt &beginit, const EndIt &endit) {
    eigen::row_check_with_assert(V, std::integer_sequence<int, 2, 3>{});

    auto it = beginit;
    auto it1 = beginit;
    it1++;
    typename Derived::Scalar vol = 0;
    mtao::Vector<typename Derived::Scalar, Derived::RowsAtCompileTime> ret(
      V.rows());
    ret.setZero();
    for (; it != endit; ++it, ++it1) {
        if (it1 == endit) {
            it1 = beginit;
        }
        auto a = V.col(*it);
        auto b = V.col(*it1);
        auto v = triangle_area(a, b);
        ret += v * (a + b) / 3;
        vol += v;
    }
    if (std::abs(vol) > 1e-10) {
        ret /= vol;
    }
    return ret;
}
template<typename Derived, typename Container>
auto curve_centroid(const Eigen::MatrixBase<Derived> &V, const Container &C) {
    return curve_centroid(V, C.begin(), C.end());
}
template<typename Derived, typename T>
auto curve_centroid(const Eigen::MatrixBase<Derived> &V,
                    std::initializer_list<T> &&C) {
    return curve_centroid(V, C.begin(), C.end());
}

/// \brief Compute the centroid using a signed cell map
/// \param V The vertices as  column vector (DxN) for R^D (D=2,3)
/// \param E the edges as a 2xN array that index into cols of V
/// \param C a signed map into E to represent a single cell
template<typename Derived, typename EDerived>
auto centroid(const Eigen::MatrixBase<Derived> &V,
              const Eigen::MatrixBase<EDerived> &E,
              const std::map<int, bool> &C) {
    eigen::row_check_with_assert(V, std::integer_sequence<int, 2, 3>{});
    mtao::Vector<typename Derived::Scalar, Derived::RowsAtCompileTime> ret(
      V.rows());
    typename Derived::Scalar vol = 0;
    ret.setZero();
    for (auto &&[eidx, sgn] : C) {
        auto e = E.col(eidx);
        auto a = V.col(e(0));
        auto b = V.col(e(1));
        auto v = (sgn ? -1 : 1) * triangle_area(a, b);
        ret += v * (a + b) / 3;
        vol += v;
    }
    if (std::abs(vol) > 1e-10) {
        ret /= vol;
    }
    return ret;
}
}// namespace mtao::geometry
#endif// CENTROID_HPP
