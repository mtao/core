#pragma once

#include <mtao/types.h>
#include <Eigen/Sparse>
#include <vector>
#include <set>
#include "mtao/geometry/mesh/cell_indexer.h"

namespace mtao::geometry::mesh {


//Takes a mesh C and its faces F and creates a matrix where each entry (i,j) points to the face comprised of every vertex except hte one at that index. i.e C(i,j) \cap F(R(i,j)) = C(j)
//R is the returned value
template<typename CellType, typename FacetType>
CellType simplex_boundary_indices(const Eigen::MatrixBase<CellType> &C, const Eigen::MatrixBase<FacetType> &F);

// takes as input a matrix from cells -> boundary facets
// returns a vector of sets, where each set is the list of indices on teh boundary 
template<typename Derived>
std::vector<std::set<int>> simplex_boundary_indices(const Eigen::SparseCompressedBase<Derived> &B);

// same as above, but for the transposed matrix
template<typename Derived>
std::vector<std::set<int>> simplex_coboundary_indices(const Eigen::SparseCompressedBase<Derived> &B);

namespace internal {
    template<bool Primal, typename Derived>
    std::vector<std::set<int>> simplex_boundary_indices_impl(const Eigen::SparseCompressedBase<Derived> &B) {
        using II = typename Derived::InnerIterator;
        std::vector<std::set<int>> ret(Primal ? B.cols() : B.rows());
        for (int o = 0; o < B.outerSize(); ++o) {
            for (II it(B, o); it; ++it) {
                if constexpr (Primal) {
                    ret[it.col()].insert(it.row());
                } else {
                    ret[it.row()].insert(it.col());
                }
            }
        }
        return ret;
    }
}// namespace internal

template<typename Derived>
std::vector<std::set<int>> simplex_boundary_indices(const Eigen::SparseCompressedBase<Derived> &B) {
    return internal::simplex_boundary_indices_impl<true>(B);
}

template<typename Derived>
std::vector<std::set<int>> simplex_coboundary_indices(const Eigen::SparseCompressedBase<Derived> &B) {
    return internal::simplex_boundary_indices_impl<false>(B);
}


template<int CellSize, typename CellType, typename FacetType>
auto simplex_boundary_indices_sized(const Eigen::MatrixBase<CellType> &C, const Eigen::MatrixBase<FacetType> &F) {


    constexpr static int FacetSize = CellSize - 1;

    static_assert((FacetSize == FacetType::RowsAtCompileTime) || (FacetType::RowsAtCompileTime == Eigen::Dynamic));
    static_assert(CellSize == FacetSize + 1);

    using Index = typename CellType::Scalar;

    using FacetArray = typename std::array<Index, FacetSize>;

    using FacetVector = typename mtao::Vector<Index, FacetSize>;

    CellIndexer<FacetSize, Index> fi(F);

    using RetType = Eigen::Matrix<Index, CellType::RowsAtCompileTime, CellType::ColsAtCompileTime>;


    RetType R = CellType::Constant(C.rows(), C.cols(), -1);

    FacetArray f;
    Eigen::Map<FacetVector> fm(f.data());
    FacetArray f2;
    Eigen::Map<FacetVector> fm2(f2.data());
    for (int i = 0; i < C.cols(); ++i) {
        auto c = C.col(i);
        Index cs = c.sum();

        std::map<int, int> cm;
        for (int i = 0; i < c.rows(); ++i) {
            cm[c(i)] = i;
        }
        auto r = R.col(i);

        fm = c.template topRows<FacetSize>();
        for (int j = 0; j < CellSize; ++j) {
            fm2 = fm;
            int fidx = fi(f2);
            auto f = F.col(fidx);


            r(cm[cs - f.sum()]) = fidx;


            auto unused = (j + FacetSize) % CellSize;
            fm(j % FacetSize) = c(unused);
        }
    }

    return R;
}

template<typename CellType, typename FacetType>
CellType simplex_boundary_indices(const Eigen::MatrixBase<CellType> &C, const Eigen::MatrixBase<FacetType> &F) {

    constexpr static int CellSize = CellType::RowsAtCompileTime;
    constexpr static int FacetSize = FacetType::RowsAtCompileTime;

    static_assert(CellSize == FacetSize + 1);

    return simplex_boundary_indices_sized<CellSize>(C, F);
}


}// namespace mtao::geometry::mesh
