#pragma once
#include <Eigen/Sparse>

#include "mtao/eigen/mat_to_triplets.hpp"
#include "mtao/eigen/type_info.hpp"

namespace mtao::eigen {
// TODO: see if doing these separately is better than the block ops
// makes a matrices of the form A,B,C
// and makes
// A   B
// C   D
template<typename T>
Eigen::SparseMatrix<T> merge_sparse_blocks(const Eigen::SparseMatrix<T> &A,
                                           const Eigen::SparseMatrix<T> &B,
                                           const Eigen::SparseMatrix<T> &C,
                                           const Eigen::SparseMatrix<T> &D) {
    using SPMat = Eigen::SparseMatrix<T>;
    std::vector<Eigen::Triplet<T>> triplets;
    triplets.reserve(A.nonZeros() + B.nonZeros() + C.nonZeros() + D.nonZeros());

    assert(A.rows() == B.rows());
    assert(C.rows() == D.rows());

    assert(A.cols() == C.cols());
    assert(B.cols() == D.cols());
    int row_offset = A.rows();
    int col_offset = A.cols();

    // TODO: see if doing these separately is better than the block ops

    for (int k = 0; k < A.outerSize(); ++k) {
        for (typename SPMat::InnerIterator it(A, k); it; ++it) {
            triplets.emplace_back(it.row(), col_offset + it.col(), it.value());
        }
    }

    for (int k = 0; k < B.outerSize(); ++k) {
        for (typename SPMat::InnerIterator it(B, k); it; ++it) {
            triplets.emplace_back(it.row(), col_offset + it.col(), it.value());
        }
    }

    for (int k = 0; k < C.outerSize(); ++k) {
        for (typename SPMat::InnerIterator it(C, k); it; ++it) {
            triplets.emplace_back(row_offset + it.row(), it.col(), it.value());
        }
    }
    for (int k = 0; k < D.outerSize(); ++k) {
        for (typename SPMat::InnerIterator it(D, k); it; ++it) {
            triplets.emplace_back(row_offset + it.row(), col_offset + it.col(), it.value());
        }
    }
    Eigen::SparseMatrix<T> R(A.rows() + C.rows(), A.cols() + B.cols());
    R.setFromTriplets(triplets.begin(), triplets.end());
    return R;
}

// A   C
// C^* B

template<typename T>
Eigen::SparseMatrix<T> merge_sparse_blocks(const Eigen::SparseMatrix<T> &A,
                                           const Eigen::SparseMatrix<T> &B,
                                           const Eigen::SparseMatrix<T> &C) {
    using SPMat = Eigen::SparseMatrix<T>;
    std::vector<Eigen::Triplet<T>> triplets;
    triplets.reserve(A.nonZeros() + B.nonZeros() + 2 * C.nonZeros());

    assert(A.rows() == A.cols());
    assert(B.rows() == B.cols());

    assert(A.rows() == C.rows());
    assert(C.cols() == B.cols());
    int row_offset = A.rows();
    int col_offset = A.cols();
    for (int k = 0; k < A.outerSize(); ++k) {
        for (typename SPMat::InnerIterator it(A, k); it; ++it) {
            triplets.emplace_back(it.row(), col_offset + it.col(), it.value());
        }
    }

    for (int k = 0; k < C.outerSize(); ++k) {
        for (typename SPMat::InnerIterator it(C, k); it; ++it) {
            triplets.emplace_back(it.row(), col_offset + it.col(), it.value());
            triplets.emplace_back(row_offset + it.col(), it.row(), it.value());
        }
    }

    for (int k = 0; k < B.outerSize(); ++k) {
        for (typename SPMat::InnerIterator it(B, k); it; ++it) {
            triplets.emplace_back(row_offset + it.row(), col_offset + it.col(), it.value());
        }
    }
    Eigen::SparseMatrix<T> R(A.rows() + B.rows(), A.cols() + B.cols());
    R.setFromTriplets(triplets.begin(), triplets.end());
    return R;
}

}// namespace mtao::eigen
