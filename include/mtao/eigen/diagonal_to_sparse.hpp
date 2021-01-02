#pragma once
#include <spdlog/spdlog.h>

#include <Eigen/Sparse>
#include <mtao/type_utils.hpp>

namespace mtao::eigen {
namespace internal {

    template<typename Derived>
    Eigen::SparseMatrix<typename Derived::Scalar> diagonal_to_sparse(
      const Derived &D) {
        using Scalar = typename Derived::Scalar;
        Eigen::SparseMatrix<Scalar> R(D.size(), D.size());
        R.reserve(Eigen::VectorXi::Constant(D.size(), 1));

        for (int j = 0; j < D.size(); ++j) {
            R.insert(j, j) = D(j);
        }
        return R;
    }
}// namespace internal

template<typename Derived>
Eigen::SparseMatrix<typename Derived::Scalar> diagonal_to_sparse(
  const Eigen::DiagonalBase<Derived> &D) {
    return internal::diagonal_to_sparse(D.diagonal());
}
template<typename Derived>
Eigen::SparseMatrix<typename Derived::Scalar> diagonal_to_sparse(
  const Eigen::MatrixBase<Derived> &D) {
    if constexpr (Derived::RowsAtCompileTime == 1 || Derived::ColsAtCompileTime == 1) {
        return internal::diagonal_to_sparse(D);
    } else if (D.rows() == 1) {
        return internal::diagonal_to_sparse(D.row(0));
    } else if (D.cols() == 1) {
        return internal::diagonal_to_sparse(D.col(0));
    } else {
        spdlog::debug(
          "diagonal_to_sparse was called on a non-vector matrix. It'll "
          "grab the diagonal for now but you should use M.diagonal() to "
          "make it clear what the diagonal sparse matrix should look "
          "like");
        return internal::diagonal_to_sparse(D.diagonal());
    }
}
}// namespace mtao::eigen
