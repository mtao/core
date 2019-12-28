#pragma once
#include <Eigen/Core>
#include <Eigen/Sparse>

namespace mtao::eigen {

    template <typename T>
        constexpr bool is_sparse(const T& A) {
            return std::is_base_of_v<
                Eigen::SparseMatrixBase<T>,T
                >;
        }
    template <typename T>
        constexpr bool is_matrix(const T& A) {
            return std::is_base_of_v<
                Eigen::MatrixBase<T>,T
                >;
        }
    template <typename T>
        constexpr bool is_array(const T& A) {
            return std::is_base_of_v<
                Eigen::ArrayBase<T>,T
                >;
        }
    template <typename T>
        constexpr bool is_row_major(const T& A) {
            return T::Options & Eigen::RowMajor;
        }
    template <typename T>
        constexpr bool is_col_major(const T& A) {
            return T::Options & Eigen::ColMajor;
        }
}
