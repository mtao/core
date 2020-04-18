#pragma once
#include <iostream>

#include "mtao/geometry/trigonometry.hpp"
//#define USE_DET_COTAN
namespace mtao::geometry {
namespace internal::winding_number {
// angle formed between A - P to B - P
template <typename PDerived, typename ADerived, typename BDerived>
typename ADerived::Scalar three_point_angle(
    const Eigen::MatrixBase<ADerived>& A, const Eigen::MatrixBase<BDerived>& B,
    const Eigen::MatrixBase<PDerived>& P) {
    using S = typename ADerived::Scalar;
#ifdef USE_DET_COTAN
    auto mytan = [](auto&& a, auto&& b) -> S {
        return (a.x() * b.y() - a.y() * b.x()) / (a.dot(b));
    };
#endif
    auto a = (A - P).eval();
    auto b = (B - P).eval();

#ifdef USE_DET_COTAN
    if (std::abs(a.dot(b)) < 1e-5) {
        continue;
    }
    S ang = mytan(a, b);

#else
    S aa = std::atan2(a.y(), a.x());
    S ba = std::atan2(b.y(), b.x());
    S ang = ba - aa;
#endif
    if (ang > M_PI) {
        ang -= 2 * M_PI;
    } else if (ang <= -M_PI) {
        ang += 2 * M_PI;
    }
    return ang;
}
}  // namespace internal::winding_number
template <typename PDerived, typename VDerived, typename BeginIt,
          typename EndIt>
typename VDerived::Scalar winding_number_iterator(const Eigen::MatrixBase<VDerived>& V,
                                         const BeginIt& beginit,
                                         const EndIt& endit,
                                         const Eigen::MatrixBase<PDerived>& p) {
    using S = typename VDerived::Scalar;
    S value = 0;
    auto it = beginit;
    for (; it != endit; ++it) {
        auto it1 = it;
        it1++;
        if (it1 == endit) {
            it1 = beginit;
        }

        auto a = V.col(*it);
        auto b = V.col(*it1);

        value += internal::winding_number::three_point_angle(a, b, p);
    }
    value /= 2 * M_PI;
    return value;
}
template <typename PDerived, typename VDerived, typename Container>
typename VDerived::Scalar winding_number(const Eigen::MatrixBase<VDerived>& V,
                                         const Container& C,
                                         const Eigen::MatrixBase<PDerived>& p) {
    return winding_number_iterator(V, C.begin(), C.end(), p);
}
template <typename PDerived, typename VDerived>
typename VDerived::Scalar winding_number(const Eigen::MatrixBase<VDerived>& V,
                                         const std::initializer_list<int>& C,
                                         const Eigen::MatrixBase<PDerived>& p) {
    return winding_number_iterator(V, C.begin(), C.end(), p);
}
template <typename PDerived, typename VDerived, typename BeginIt,
          typename EndIt>
bool interior_winding_number_iterator(const Eigen::MatrixBase<VDerived>& V,
                             const BeginIt& beginit, const EndIt& endit,
                             const Eigen::MatrixBase<PDerived>& p) {
    auto v = winding_number_iterator(V, beginit, endit, p);
    return std::abs(v) > .5;
}
template <typename PDerived, typename VDerived, typename Container>
bool interior_winding_number(const Eigen::MatrixBase<VDerived>& V,
                             const Container& C,
                             const Eigen::MatrixBase<PDerived>& p) {
    return interior_winding_number_iterator(V, C.begin(), C.end(), p);
}
template <typename PDerived, typename VDerived>
bool interior_winding_number_iterator(const Eigen::MatrixBase<VDerived>& V,
                             const std::initializer_list<int>& C,
                             const Eigen::MatrixBase<PDerived>& p) {
    return interior_winding_number(V, C.begin(), C.end(), p);
}

template <typename PDerived, typename VDerived, typename EDerived>
double winding_number(const Eigen::MatrixBase<VDerived>& V,
                    const Eigen::MatrixBase<EDerived>& E,
                    const std::map<int, bool>& boundary_map,
                    const Eigen::MatrixBase<PDerived>& p) {
    using S = typename VDerived::Scalar;
    S value = 0;
    for (auto&& [idx, sgn] : boundary_map) {
        auto e = E.col(idx);
        int ai, bi;
        if (sgn) {
            ai = e(0);
            bi = e(1);
        } else {
            ai = e(1);
            bi = e(0);
        }
        auto a = V.col(ai);
        auto b = V.col(bi);

        value += internal::winding_number::three_point_angle(a, b, p);
        //std::cout << "{" << value << "}";
    }
    value /= 2 * M_PI;
    return value;
}
template <typename PDerived, typename VDerived, typename EDerived>
bool interior_winding_number(const Eigen::MatrixBase<VDerived>& V,
                    const Eigen::MatrixBase<EDerived>& E,
                    const std::map<int, bool>& boundary_map,
                    const Eigen::MatrixBase<PDerived>& p) {

    auto v = winding_number_iterator(V, E, boundary_map, p);
    return std::abs(v) < .5;
}
}  // namespace mtao::geometry
