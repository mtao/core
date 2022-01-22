
#pragma once

#include <concepts>
#include <Eigen/Core>
#include <Eigen/Sparse>

namespace mtao::eigen::concepts {

namespace internal {
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


    template<typename T>
    using derived_type = std::decay_t<decltype(std::declval<T>().derived())>;
}// namespace internal
template<typename T>
concept MatrixBaseDerived = internal::MatrixBaseDerived<internal::derived_type<T>>;

template<typename T>
concept ArrayBaseDerived = internal::ArrayBaseDerived<internal::derived_type<T>>;

template<typename T>
concept DenseBaseDerived = internal::DenseBaseDerived<internal::derived_type<T>>;


template<typename T>
concept PlainObjectBaseDerived = internal::PlainObjectBaseDerived<internal::derived_type<T>>;

template<typename T>
concept EigenBaseDerived = internal::EigenBaseDerived<internal::derived_type<T>>;

template<typename T>
concept SparseCompressedBaseDerived = std::derived_from<T, typename Eigen::SparseCompressedBase<T>>;

template<typename T>
concept SparseMatrixBaseDerived = std::derived_from<T, typename Eigen::SparseMatrixBase<T>>;

template <typename T>
concept IsEigenMatrix = MatrixBaseDerived<T> || SparseMatrixBaseDerived<T>;

}// namespace mtao::eigen::concepts

