#pragma once
#include <iostream>
#include "mtao/geometry/trigonometry.hpp"
//#define USE_DET_COTAN
namespace mtao::geometry {
    template <typename PDerived, typename VDerived, typename BeginIt, typename EndIt>
    typename VDerived::Scalar winding_number(const Eigen::MatrixBase<VDerived>& V, const BeginIt& beginit, const EndIt& endit, const Eigen::MatrixBase<PDerived>& p) {
        using S = typename VDerived::Scalar;
#ifdef USE_DET_COTAN
        auto mytan = [](auto&& a, auto&& b) -> S{
            return (a.x() * b.y() - a.y() * b.x()) / (a.dot(b));
        };
#endif
        S value = 0;
        auto it = beginit;
        for(; it != endit; ++it) {
            auto it1 = it;
            it1++;
            if(it1 == endit) { it1 = beginit; }

            auto a = (V.col(*it) - p).eval();
            auto b = (V.col(*it1) - p).eval();

            S aa = std::atan2(a.y(),a.x());
            S ba = std::atan2(b.y(),b.x());
#ifdef USE_DET_COTAN
            if(std::abs(a.dot(b)) < 1e-5) {
                continue;
            }
            S ang = mytan(a,b);

#else
            S ang = ba - aa;
#endif
            if(ang > M_PI) {
                ang -= 2 * M_PI;
            } else if(ang <= -M_PI) {
                ang += 2*M_PI;
            }
            value += ang;

        }
        return value;
    }
    template <typename PDerived, typename VDerived, typename Container>
    typename VDerived::Scalar winding_number(const Eigen::MatrixBase<VDerived>& V, const Container& C, const Eigen::MatrixBase<PDerived>& p) {
        return winding_number(V,C.begin(),C.end(),p);
    }
    template <typename PDerived, typename VDerived>
    typename VDerived::Scalar winding_number(const Eigen::MatrixBase<VDerived>& V, const std::initializer_list<int>& C, const Eigen::MatrixBase<PDerived>& p) {
        return winding_number(V,C.begin(),C.end(),p);
    }
    template <typename PDerived, typename VDerived, typename BeginIt, typename EndIt>
    bool interior_winding_number(const Eigen::MatrixBase<VDerived>& V, const BeginIt& beginit, const EndIt& endit, const Eigen::MatrixBase<PDerived>& p) {
        auto v = winding_number(V,beginit,endit,p);
        return std::abs(v) > 1;
    }
    template <typename PDerived, typename VDerived, typename Container>
    bool interior_winding_number(const Eigen::MatrixBase<VDerived>& V, const Container& C, const Eigen::MatrixBase<PDerived>& p) {
        return interior_winding_number(V,C.begin(),C.end(),p);
    }
    template <typename PDerived, typename VDerived>
    bool interior_winding_number(const Eigen::MatrixBase<VDerived>& V, const std::initializer_list<int>& C, const Eigen::MatrixBase<PDerived>& p) {
        return interior_winding_number(V,C.begin(),C.end(),p);
    }
}
