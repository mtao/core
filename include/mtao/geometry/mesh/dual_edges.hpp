#pragma once
#include <array>
#include <map>
// TODO: WARN SOPMEONE SOMETHING BAD HAS HAPPENEd
#include <set>
#include <vector>

#include "mtao/iterator/enumerate.hpp"
#include "mtao/eigen/stl2eigen.hpp"

namespace mtao::geometry::mesh {

// given cells and their boundaries, determine the dual edges with the same
// ordering as the boundaries i.e the dual edge stored in col j = the dual for
// face at col j
// beware that we may have entries like -1
template <int D, typename CellType, typename FacetType>
auto dual_edges(const Eigen::MatrixBase<CellType>& C,
                const Eigen::MatrixBase<FacetType>& F) {
    using BSim = std::array<int, D-1>;
    std::map<BSim, int> index;
    std::vector<std::set<int>> DE(F.cols());
    auto get_bsimplex = [&](int i, auto&& col) {
        BSim ret;
        for (int j = 0; j < D-1; ++j) {
            int idx = (i + j + 1) % D;
            ret[j] = col(idx);
        }
        std::sort(ret.begin(), ret.end());
        return ret;
    };
    for (int i = 0; i < F.cols(); ++i) {
        BSim b;
        mtao::eigen::stl2eigen(b) = F.col(i);
        std::sort(b.begin(),b.end());
        index[b] = i;
    }
    for (int i = 0; i < C.cols(); ++i) {
        for (int j = 0; j < C.rows(); ++j) {
            BSim bsim = get_bsimplex(j, C.col(i));
            DE[index.at(bsim)].insert(i);
        }
    }
    mtao::ColVectors<int, 2> E =
        mtao::ColVectors<int, 2>::Constant(2, F.cols(), -1);
    for (size_t i = 0; i < DE.size(); ++i) {
        auto e = E.col(i);
        auto& de = DE.at(i);
        for (auto [idx, val] : mtao::iterator::enumerate(de)) {
            if(idx < 2) {
            e(idx) = val;
            } else {
                // TODO: WARN SOPMEONE SOMETHING BAD HAS HAPPENEd
            }
        }
    }
    return E;
}

}  // namespace mtao::geometry::mesh
