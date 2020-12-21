#pragma once
#include <Eigen/Sparse>
#include <mtao/type_utils.hpp>

namespace mtao::eigen {
namespace internal {

template <typename Derived>
Eigen::SparseMatrix<typename Derived::Scalar> diagonal_to_sparse(
    const Derived& D) {
    using Scalar = typename Derived::Scalar;
    Eigen::SparseMatrix<Scalar> R(D.size(), D.size());
    R.reserve(Eigen::VectorXi::Constant(D.size(), 1));

    for (int j = 0; j < D.size(); ++j) {
        R.insert(j, j) = D(j);
    }
    return R;
}
}  // namespace internal

template <typename Derived>
Eigen::SparseMatrix<typename Derived::Scalar> diagonal_to_sparse(
    const Eigen::DiagonalBase<Derived>& D) {
    return internal::diagonal_to_sparse(D.diagonal());
}
template <typename Derived>
Eigen::SparseMatrix<typename Derived::Scalar> diagonal_to_sparse(
    const Eigen::MatrixBase<Derived>& D) {
    return internal::diagonal_to_sparse(D);
}
}  // namespace mtao::eigen
