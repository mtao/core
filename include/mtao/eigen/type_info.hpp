#pragma once
#include <Eigen/Core>
#include <Eigen/Sparse>

namespace mtao::eigen {

template<typename T>
constexpr bool is_sparse() {
    return std::is_base_of_v<
      Eigen::SparseMatrixBase<T>,
      T>;
}
template<typename T>
constexpr bool is_matrix() {
    return std::is_base_of_v<
      Eigen::MatrixBase<T>,
      T>;
}
template<typename T>
constexpr bool is_array() {
    return std::is_base_of_v<
      Eigen::ArrayBase<T>,
      T>;
}
template<typename T>
constexpr bool is_row_major() {
    return T::Options & Eigen::RowMajor;
}
template<typename T>
constexpr bool is_col_major() {
    return T::Options & Eigen::ColMajor;
}
template<typename T>
constexpr bool is_sparse(const T &A) {
    return is_sparse<T>();
}
template<typename T>
constexpr bool is_matrix(const T &A) {
    return is_matrix<T>();
}
template<typename T>
constexpr bool is_array(const T &A) {
    return is_array<T>();
}
template<typename T>
constexpr bool is_row_major(const T &A) {
    return is_row_major<T>();
}
template<typename T>
constexpr bool is_col_major(const T &A) {
    return is_col_major<T>();
}
}// namespace mtao::eigen
