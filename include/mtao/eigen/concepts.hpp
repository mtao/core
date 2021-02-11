
#pragma once

#include <concepts>
#include <Eigen/Core>
#include <Eigen/Sparse>

namespace mtao::eigen::concepts {

template<typename T>
concept MatrixBaseDerived = std::derived_from<T, typename Eigen::MatrixBase<T>>;

template<typename T>
concept ArrayBaseDerived = std::derived_from<T, typename Eigen::ArrayBase<T>>;

template<typename T>
concept DenseBaseDerived = std::derived_from<T, typename Eigen::DenseBase<T>>;


template<typename T>
concept PlainObjectBaseDerived = std::derived_from<T, typename Eigen::PlainObjectBase<T>>;

template<typename T>
concept EigenBaseDerived = std::derived_from<T, typename Eigen::EigenBase<T>>;

template<typename T>
concept SparseCompressedBaseDerived = std::derived_from<T, typename Eigen::SparseCompressedBase<T>>;

template<typename T>
concept SparseMatrixBaseDerived = std::derived_from<T, typename Eigen::SparseMatrixBase<T>>;


}// namespace mtao::eigen::concepts

