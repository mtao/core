#pragma once
#include <map>
#include <array>
#include <vector>
#include <set>
#include "mtao/iterator/enumerate.hpp"

namespace mtao::geometry::mesh {

    template <int D, typename CellType, typename FacetType>
        auto dual_edges(const Eigen::MatrixBase<CellType>& C, const Eigen::MatrixBase<FacetType>& F) {
            using BSim = std::array<int,D>;
            std::map<BSim,int> index;
            std::vector<std::set<int>> DE(F.cols());
            auto get_bsimplex = [&](int i, auto&& col) {
                BSim ret;
                for(int j = 0; j < D; ++j) {
                    int idx = (i+j+1)%D;
                    ret[j] = col(idx);
                }
                std::sort(ret.begin(),ret.end());
            };
            for(int i = 0; i < F.cols(); ++i) {
                index[get_bsimplex(0,F.col(i))] = i;
            }
            for(int i = 0; i < C.cols(); ++i) {
                for(int j = 0; i < C.rows(); ++j) {
                    DE[index[get_bsimplex(j,F.col(i))]].insert(i);
                }
            }
            mtao::ColVectors<int,2> E = mtao::ColVectors<int,2>::Constant(2,C.cols(),-1);
            for(int i = 0; i < DE.size(); ++i) {
                auto& e = E.col(i);
                auto& de = DE[i];
                for(auto [idx,val]: de) {
                    e(idx) = val;
                }

            }
            return E;
        }

}
