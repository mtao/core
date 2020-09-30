#pragma once
#include <mtao/eigen/mat_to_triplets.hpp>
#include <mtao/iterator/shell.hpp>

namespace mtao::eigen {

// generates a block-diagonal matrix where each block is the input matrix
template <typename Scalar, int Options, typename StorageIndex>
auto sparse_block_diagonal_repmats(
    const Eigen::SparseMatrix<Scalar, Options, StorageIndex>& A,
    int num_repeats) {
    Eigen::SparseMatrix<Scalar, Options, StorageIndex> R(
        num_repeats * A.rows(), num_repeats * A.cols());
    std::vector<Eigen::Triplet<double>> trips = mtao::eigen::mat_to_triplets(A);

    trips.reserve(num_repeats * A.nonZeros());
    for (int j = 1; j < num_repeats; ++j) {
        for (auto&& t : mtao::iterator::shell(trips.begin(),
                                              trips.begin() + A.nonZeros())) {
            trips.emplace_back(t.row() + j * A.rows(), t.col() + j * A.cols(),
                               t.value());
        }
    }
    R.setFromTriplets(trips.begin(), trips.end());
    return R;
}

}  // namespace mtao::eigen

