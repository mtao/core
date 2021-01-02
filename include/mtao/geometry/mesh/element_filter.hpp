#pragma once

#include <vector>
#include "mtao/enumerate.hpp"
#include "mtao/types.h"
#include "mtao/eigen/slice_filter.hpp"


namespace mtao {
namespace geometry {
    namespace mesh {


        //For each element in C, if every vertex satisfies f(v) then true, false o.w
        template<typename T, int D, int D2, typename Func>
        auto element_filter_mask(const mtao::ColVectors<T, D> &V, const mtao::ColVectors<int, D2> &C, const Func &f) {
            mtao::VectorX<bool> B(C.cols());
            for (int i = 0; i < C.cols(); ++i) {
                bool enabled = true;
                auto c = C.col(i);
                for (int j = 0; j < C.rows(); ++j) {
                    if (!f(V.col(c(j)))) {
                        enabled = false;
                    }
                }
                B(i) = enabled;
            }
            return B;
        }
        //For each element in C, if every vertex satisfies f(v) then it remains, tossed o.w
        template<typename T, int D, int D2, typename Func>
        auto element_filter(const mtao::ColVectors<T, D> &V, const mtao::ColVectors<int, D2> &C, const Func &f) {
            return mtao::eigen::slice_filter_col(C, element_filter_mask(V, C, f));
        }


    }// namespace mesh
}// namespace geometry
}// namespace mtao
