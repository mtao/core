#pragma once

#include "mtao/types.hpp"
#include <vector>
#include <set>
#include <map>
#include "mtao/geometry/mesh/simplex_boundary_indices.hpp"
#include "mtao/iterator/enumerate.hpp"
#include "mtao/eigen/stl2eigen.hpp"

namespace mtao::geometry::mesh {

// Given a boundary_index->sign representation of a mesh compute the cell boundaries
// Non-matching boundaries are counted
std::set<int> boundary_element_indices(const std::vector<std::map<int, bool>> &cells);


// Given a colvectors mapping simplex indices into boundary simplex indices, constructs the list of boundary simplex indices that lie on the boundary of the simplicial complex
template<typename CellBoundary>
std::set<int> boundary_element_indices(const Eigen::MatrixBase<CellBoundary> &CB);


// Given a colvectors of simplices and colvector of boundary simplices, constructs the list of boundary simplex indices that lie on the boundary of the simplicial complex
template<typename CellType, typename FacetType>
std::set<int> boundary_element_indices(const Eigen::MatrixBase<CellType> &C, const Eigen::MatrixBase<FacetType> &F);

template<typename CellType, typename FacetType>
[[deprecated]] CellType boundary_elements(const Eigen::MatrixBase<CellType> &C, const Eigen::MatrixBase<FacetType> &F) {
    return simplex_boundary_indices(C, F);
}


template<typename CellType, typename FacetType>
std::set<int> boundary_element_indices(const Eigen::MatrixBase<CellType> &C, const Eigen::MatrixBase<FacetType> &F) {
    auto BE = simplex_boundary_indices(C, F);
    return boundary_element_indices(BE);
}

template<typename BoundaryElements>
std::set<int> boundary_element_indices(const Eigen::MatrixBase<BoundaryElements> &BE) {
    std::vector<int> counts(BE.maxCoeff() + 1, 0);
    for (int j = 0; j < BE.size(); ++j) {
        counts[BE(j)]++;
    }
    std::set<int> good_counts;
    for (auto &&[idx, count] : mtao::iterator::enumerate(counts)) {
        if (count == 1) {
            good_counts.emplace(idx);
        }
    }
    return good_counts;
}

}// namespace mtao::geometry::mesh
