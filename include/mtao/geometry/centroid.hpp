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
/// In 2d this works with immersions because triangle_area is signed, in 3d not so much
template<typename Derived, typename BeginIt, typename EndIt>
auto curve_centroid(const Eigen::MatrixBase<Derived> &V, const BeginIt &beginit, const EndIt &endit) {
    eigen::row_check_with_throw(V, std::integer_sequence<int, 2, 3>{});

    auto it = beginit;
    if (std::distance(beginit, endit) < 3) {
        throw std::runtime_error("Tried to triangulate a curve with less than 3 vertices");
    }

    // when the dim is not 2 we want to use an in-plane triangle fan - so we want to skip one triangle
    // it would be nice if
    //if constexpr (!eigen::concepts::ColVecs2Compatible<Derived>) {
    //    ++it;
    //}
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
        typename Derived::Scalar v;
        if constexpr (eigen::concepts::ColVecs2Compatible<Derived>) {
            v = triangle_area(a, b);
            ret += v * (a + b) / 3;
        } else {
            auto c = V.col(*beginit);
            v = triangle_area(a, b, c);
            ret += v * (a + b + c) / 3;
        }
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
template<eigen::concepts::ColVecs2Compatible Vertices, eigen::concepts::ColVecs2Compatible Edges>
auto centroid(const Vertices &V,
              const Edges &E,
              const std::map<int, bool> &C) {
    eigen::row_check_with_throw<2>(V);
    //eigen::row_check_with_throw(V, std::integer_sequence<int, 2, 3>{});
    mtao::Vector<typename Vertices::Scalar, Vertices::RowsAtCompileTime> ret(
      V.rows());
    typename Vertices::Scalar vol = 0;
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
