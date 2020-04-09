#ifndef CENTROID_HPP
#define CENTROID_HPP
#include "mtao/types.hpp"
#include "mtao/util.h"
#include <numeric>
#include <cassert>

namespace mtao::geometry {
    template <typename Derived, typename BeginIt, typename EndIt>
        auto curve_centroid( const Eigen::MatrixBase<Derived> & V, const BeginIt& beginit, const EndIt& endit) {
            static_assert(
                    Derived::RowsAtCompileTime == 2 || 
                    Derived::RowsAtCompileTime == 3 || 
                    Derived::RowsAtCompileTime == Eigen::Dynamic);
            auto it = beginit;
            auto it1 = beginit;
            it1++;
            typename Derived::Scalar vol = 0;
            mtao::Vector<typename Derived::Scalar,Derived::RowsAtCompileTime> ret(V.rows());
            ret.setZero();
            for(; it != endit; ++it, ++it1) {
                if(it1 == endit) {
                    it1 = beginit;
                }
                auto a = V.col(*it);
                auto b = V.col(*it1);
                typename Derived::Scalar v;
                if constexpr(Derived::RowsAtCompileTime == 2) {
                    v = a.x() * b.y() - a.y() * b.x();
                } else if constexpr(Derived::RowsAtCompileTime == 3) {
                    v = a.cross(b).norm();
                } else if constexpr(Derived::RowsAtCompileTime == Eigen::Dynamic) {
                    if(V.rows() == 2) {
                        v = a.x() * b.y() - a.y() * b.x();
                    } else if(V.rows() == 3) {

                        v = a.cross(b).norm();
                    }
                }
                vol += v;
                ret += v * (a + b) / 3;


            }
            if(vol > 1e-10) {
            ret /= vol;
            }
            return ret;


        }
    template <typename Derived, typename Container>
        auto curve_centroid( const Eigen::MatrixBase<Derived> & V, const Container& C) {
            return curve_centroid(V,C.begin(),C.end());
        }
}
#endif//CENTROID_HPP
