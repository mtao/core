#pragma once

#include <vector>
#include "mtao/enumerate.hpp"
#include "mtao/types.h"


namespace mtao { namespace geometry { namespace mesh {


    template <typename T, int D, int D2, typename BoolVec>
        auto element_filter_vec(const mtao::ColVectors<T,D>& V, const mtao::ColVectors<int,D2>& C, const Eigen::DenseBase<BoolVec>& B) {
            std::vector<int> EA;
            for(int i = 0; i < B.rows(); ++i) {
                if(B(i)) {
                    EA.push_back(i);
                }
            }
            mtao::ColVectors<int,D2> R(C.rows(), EA.size());

            for(auto&& [ni,oi]: mtao::enumerate(EA)) {
                R.col(ni) = C.col(oi);
            }
            return R;
        }
    //For each element in C, if every vertex satisfies f(v) then true, false o.w
    template <typename T, int D, int D2, typename Func>
        auto element_filter_mask(const mtao::ColVectors<T,D>& V, const mtao::ColVectors<int,D2>& C, const Func& f) {
            mtao::VectorX<bool> B(C.cols());
            for(int i = 0; i < C.cols(); ++i) {
                bool enabled = true;
                auto c = C.col(i);
                for(int j = 0; j < C.rows(); ++j) {
                    if(!f(V.col(c(j)))) {
                        enabled = false;
                    }
                }
                B(i) = enabled;
            }
            return B;
        }
    //For each element in C, if every vertex satisfies f(v) then it remains, tossed o.w
    template <typename T, int D, int D2, typename Func>
        auto element_filter(const mtao::ColVectors<T,D>& V, const mtao::ColVectors<int,D2>& C, const Func& f) {
            return element_filter_vec(V,C,element_filter_mask(V,C,f));
        }


}}}
