#pragma once
#include <array>
#include <map>
#include <set>
#include <vector>

#include "mtao/geometry/circumcenter.hpp"
#include "mtao/geometry/mesh/dual_edges.hpp"

namespace mtao::geometry::mesh {

// the circumcentral dual edge lengths
// takes in vertices, cells, and boundaries
template<typename VType, typename CellType, typename FacetType>
auto dual_edge_lengths(const Eigen::MatrixBase<VType> &V,
                       const Eigen::MatrixBase<CellType> &C,
                       const Eigen::MatrixBase<FacetType> &F) {
    auto DE = dual_edges<CellType::RowsAtCompileTime>(C, F);
    auto CC = circumcenters(V, C);

    using Scalar = typename VType::Scalar;
    VectorX<Scalar> R(F.cols());
    for (int j = 0; j < R.size(); ++j) {
        auto de = DE.col(j);

        if (de(1) == -1) {
            if (de(0) == -1) {
                continue;
            }
            R(j) = (circumcenters(V, F.col(j)) - CC.col(de(0))).norm();
        } else {
            R(j) = (CC.col(de(1)) - CC.col(de(0))).norm();
        }
    }
    return R;
}

}// namespace mtao::geometry::mesh
