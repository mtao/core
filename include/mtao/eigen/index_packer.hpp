#pragma once
#include <Eigen/Dense>
#include "mtao/iterator/enumerate.hpp"

namespace mtao::eigen {
    //Packs a ColVector type matrix V and writes it to a new matrix according to an indexer indices
    template <typename Derived>
        auto index_packer(const Eigen::EigenBase<Derived>& V, const std::vector<int>& indices) {
            Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Eigen::Dynamic> R(V.rows(), indices.size());
            for(auto&& [i,v]: indices) {
                R.col(i) = V.col(v);
            }
            return R;
        }
    //Packs a ColVector type matrix V and writes it to a new matrix according to an indexer indices
    template <typename Derived, int D>
        auto index_packer(const Eigen::EigenBase<Derived>& V, const std::array<int,D>& indices) {
            Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, D> R(V.rows(), indices.size());
            for(auto&& [i,v]: indices) {
                R.col(i) = V.col(v);
            }
            return R;
        }
}
