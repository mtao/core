#pragma once

#include <mtao/types.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <set>

#include "mtao/algebra/sort_with_permutation_sign.hpp"
#include "mtao/eigen/stl2eigen.hpp"

namespace mtao::geometry::mesh {

// Only for simplices
template<typename CellType>
auto unique_simplices(const Eigen::MatrixBase<CellType> &C) ->
  typename CellType::PlainMatrix {
    // each simplex can be uniquely canonically represented by
    // itself sorted and whether it took an even or odd permutation to obtain it
    // Because we're assuming simplices of dimension <= 4 bubble sort is not
    // only the most efficient, its easy to impl and check the number of swaps
    // :)
    constexpr int Rows = CellType::RowsAtCompileTime;
    constexpr bool DynamicSize = Rows == Eigen::Dynamic;
    using StlType = std::conditional_t<DynamicSize, std::vector<int>, std::array<int, Rows>>;

    std::map<StlType, bool> uniq_map;

    for (int i = 0; i < C.cols(); ++i) {
        if constexpr (DynamicSize) {
            StlType s(C.rows());
            mtao::eigen::stl2eigen(s) = C.col(i);
            bool even = algebra::sort_with_permutation_sign_in_place<
              algebra::SortType::Bubble>(s);

            uniq_map.try_emplace(std::move(s), even);
        } else {
            StlType s;
            mtao::eigen::stl2eigen(s) = C.col(i);
            bool even = algebra::sort_with_permutation_sign_in_place<
              algebra::SortType::Bubble>(s);

            uniq_map.try_emplace(std::move(s), even);
        }
    }

    // if  the input simplices were unique lets avoid messing with the input
    if (int(uniq_map.size()) != C.cols()) {
        typename CellType::PlainMatrix CC(C.rows(), uniq_map.size());
        for (auto &&[idx, pr] : mtao::iterator::enumerate(uniq_map)) {
            auto &&[c, even] = pr;
            auto v = CC.col(idx) = mtao::eigen::stl2eigen(c);
            if (C.rows() > 1 && !even) {
                std::swap(v(0), v(1));
            }
        }
        return CC;
    }
    return C;
}

}// namespace mtao::geometry::mesh
