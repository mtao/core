#pragma once
#include "mtao/eigen/partition_matrix.hpp"
namespace mtao::eigen {

template <typename Derived, typename IndexContainer>
auto partition_vector(const Eigen::MatrixBase<Derived>& M,
                      const IndexContainer& indices) {
    if constexpr (Derived::RowsAtCompileTime == 1) {
        return detail::_partition_inner_stride<false>(
            std::make_integer_sequence<int,
                                       std::tuple_size_v<IndexContainer> - 1>{},
            M, indices);
    } else if constexpr (Derived::ColsAtCompileTime == 1) {
        return detail::_partition_inner_stride<true>(
            std::make_integer_sequence<int,
                                       std::tuple_size_v<IndexContainer> - 1>{},
            M, indices);
    } else {
        return detail::_partition_inner_stride<true>(
            std::make_integer_sequence<int,
                                       std::tuple_size_v<IndexContainer> - 1>{},
            M, indices);
    }
}
template <typename Derived, typename IndexContainer>
auto partition_vector(Eigen::PlainObjectBase<Derived>& M,
                      const IndexContainer& indices) {
    if constexpr (Derived::RowsAtCompileTime == 1) {
        return detail::_partition_inner_stride_writable<false>(
            std::make_integer_sequence<int,
                                       std::tuple_size_v<IndexContainer> - 1>{},
            M, indices, 0, 0);
    } else if constexpr (Derived::ColsAtCompileTime == 1) {
        return detail::_partition_inner_stride_writable<true>(
            std::make_integer_sequence<int,
                                       std::tuple_size_v<IndexContainer> - 1>{},
            M, indices, 0, 0);
    } else {
        return detail::_partition_inner_stride_writable<true>(
            std::make_integer_sequence<int,
                                       std::tuple_size_v<IndexContainer> - 1>{},
            M, indices, 0, 0);
    }
}
/*
template <typename Derived, typename IndexContainer>
auto partition_vector_from_size(const Eigen::MatrixBase<Derived>& M,
                                const IndexContainer& indices) {
    std::array<IndexContainer::value_type,
               std::tuple_size_v<IndexContainer> + 1>
        arr;
    return partition_vector(M, indices);
}
*/
}  // namespace mtao::eigen
