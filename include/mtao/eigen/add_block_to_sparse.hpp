#pragma once

namespace mtao::eigen {
// Eigen doesn't quite like adding blocks to a single matrix
// This is not efficient for reapatedly adding blocks

template <typename SparseType, typename SubType>
auto add_block_to_sparse(const Eigen::SparseMatrixBase<SparseType>& A,
                         const SubType& B, int row, int col) {
    using Scalar = typename SparseType::Scalar;
    constexpr static int Options = SparseType::Options;
    SparseType L(A.rows(), B.rows());
    SparseType R(B.cols(), A.cols());

    for (int k = 0; k < B.rows(); ++k) {
        L.insert(row + k, k) = 1.;
    }
    for (int k = 0; k < B.cols(); ++k) {
        R.insert(k, col + k) = 1.;
    }
    auto SB = SparseType(B.sparseView());

    return (L * SB * R).eval();
}
}  // namespace mtao::eigen
