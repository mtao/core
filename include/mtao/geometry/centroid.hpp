#ifndef CENTROID_HPP
#define CENTROID_HPP
#include <Eigen/Core>
#include <cassert>
#include <numeric>

#include "mtao/types.hpp"
#include "mtao/util.h"

namespace mtao::geometry {
template <typename Derived, typename BeginIt, typename EndIt>
auto curve_centroid(const Eigen::MatrixBase<Derived>& V, const BeginIt& beginit,
                    const EndIt& endit) {
    static_assert(Derived::RowsAtCompileTime == 2 ||
                  Derived::RowsAtCompileTime == 3 ||
                  Derived::RowsAtCompileTime == Eigen::Dynamic);
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
    if (vol > 1e-10) {
        ret /= vol;
    }
    return ret;
}
template <typename Derived, typename Container>
auto curve_centroid(const Eigen::MatrixBase<Derived>& V, const Container& C) {
    return curve_centroid(V, C.begin(), C.end());
}

/// \brief Compute the centroid using a signed cell map
/// \param V The vertices as  column vector (DxN) for R^D (D=2,3)
/// \param E the edges as a 2xN array that index into cols of V
/// \param C a signed map into E to represent a single cell
template <typename Derived, typename EDerived>
auto curve_centroid(const Eigen::MatrixBase<Derived>& V,
                    const Eigen::MatrixBase<EDerived>& E,
                    const std::map<int, bool>& C) {
    for (auto&& [eidx, sgn] : C) {
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
        if (vol > 1e-10) {
            ret /= vol;
        }
    }
}  // namespace mtao::geometry
#endif  // CENTROID_HPP
