#pragma once
#include <Eigen/Core>
#include <iostream>
#include <tuple>

#include "mtao/type_utils.hpp"
#include "mtao/utils/partial_sum.hpp"
namespace mtao::eigen {
// for matrix-like blocks partition_matrix generates a tuple structure like
// { {a,b} {c,d} } for
// [ A B ]
// [ C D ]
// for vector-like cases it simply generates
// {a,b} for either
// [ A B ]
// or
// [ A ]
// [ B ]
//
// the default variant takes the offsets, presumably starting with 0 and the
// size This is equivalent to setting the boundary of partitions, i.e indices
// {0,1,5} generates 2 partitions from [0,1) and [1,5).
//
// the *_from_sizes variants take the sizes of the blocks themselves

namespace detail {

// start/end are required for PlainObjectBase because Blocks have trouble
// transferring writeability
template <bool UseRow, int... N, typename Derived,
          typename IndexContainer = mtao::types::empty_t>
auto _partition_inner_stride_writable(std::integer_sequence<int, N...>,
                                      Eigen::PlainObjectBase<Derived>& M,
                                      const IndexContainer& indices, int start,
                                      int end) {
    if constexpr (std::is_same_v<IndexContainer, types::empty_t>) {
        if constexpr (Derived::RowsAtCompileTime == Eigen::Dynamic &&
                      Derived::ColsAtCompileTime == Eigen::Dynamic) {
            return M.template block<Derived::RowsAtCompileTime,
                                    Derived::ColsAtCompileTime>(0, 0);
        } else {
            return M.block(0, 0, M.rows(), M.cols());
        }
    }
    if constexpr (UseRow) {
        if constexpr (Derived::ColsAtCompileTime == 1) {
            return std::make_tuple(
                // M.head(std::get<0>(indices),
                M.segment(std::get<N>(indices),
                          std::get<N + 1>(indices) - std::get<N>(indices))...);
        } else {
            return std::make_tuple(
                // M.head(std::get<0>(indices),
                M.block(std::get<N>(indices), start,
                        std::get<N + 1>(indices) - std::get<N>(indices),
                        end)...);
        }
    } else {
        if constexpr (Derived::RowsAtCompileTime == 1) {
            return std::make_tuple(
                // M.head(std::get<0>(indices),
                M.segment(std::get<N>(indices),
                          std::get<N + 1>(indices) - std::get<N>(indices))...);
        } else {
            return std::make_tuple(
                // M.head(std::get<0>(indices),
                M.block(start, std::get<N>(indices), end,
                        std::get<N + 1>(indices) - std::get<N>(indices))...);
        }
    }
}

template <bool UseRow, int... N, typename Derived, typename RowIndexContainer,
          typename ColIndexContainer>
auto _partition_outer_stride_writable(std::integer_sequence<int, N...>,
                                      Eigen::PlainObjectBase<Derived>& M,
                                      const RowIndexContainer& rows,
                                      const ColIndexContainer& cols) {
    if constexpr (UseRow) {
        if constexpr (std::is_same_v<RowIndexContainer, types::empty_t>) {
            return _partition_inner_stride<false>(M, cols);
        } else {
            return std::make_tuple(_partition_inner_stride_writable<false>(
                std::make_integer_sequence<
                    int, std::tuple_size_v<ColIndexContainer> - 1>{},
                M, cols, std::get<N>(rows),
                std::get<N + 1>(rows) - std::get<N>(rows))...);
        }
    } else {
        if constexpr (std::is_same_v<ColIndexContainer, types::empty_t>) {
            return _partition_inner_stride<true>(M, rows);
        } else {
            return std::make_tuple(_partition_inner_stride<true>(
                std::make_integer_sequence<
                    int, std::tuple_size_v<ColIndexContainer> - 1>{},
                M, rows, std::get<N>(cols),
                std::get<N + 1>(cols) - std::get<N>(cols))...);
        }
    }
}

template <bool UseRow, int... N, typename Derived,
          typename IndexContainer = mtao::types::empty_t>
auto _partition_inner_stride(std::integer_sequence<int, N...>,
                             const Eigen::MatrixBase<Derived>& M,
                             const IndexContainer& indices) {
    if constexpr (std::is_same_v<IndexContainer, types::empty_t>) {
        if constexpr (Derived::RowsAtCompileTime == Eigen::Dynamic &&
                      Derived::ColsAtCompileTime == Eigen::Dynamic) {
            return M.template block<Derived::RowsAtCompileTime,
                                    Derived::ColsAtCompileTime>(0, 0);
        } else {
            return M.block(0, 0, M.rows(), M.cols());
        }
    }
    if constexpr (UseRow) {
        if constexpr (Derived::ColsAtCompileTime == 1) {
            return std::make_tuple(
                // M.head(std::get<0>(indices),
                M.segment(std::get<N>(indices),
                          std::get<N + 1>(indices) - std::get<N>(indices))...);
        } else {
            return std::make_tuple(
                // M.head(std::get<0>(indices),
                M.block(std::get<N>(indices), 0,
                        std::get<N + 1>(indices) - std::get<N>(indices),
                        M.cols())...);
        }
    } else {
        if constexpr (Derived::RowsAtCompileTime == 1) {
            return std::make_tuple(
                // M.head(std::get<0>(indices),
                M.segment(std::get<N>(indices),
                          std::get<N + 1>(indices) - std::get<N>(indices))...);
        } else {
            return std::make_tuple(
                // M.head(std::get<0>(indices),
                M.block(0, std::get<N>(indices), M.rows(),
                        std::get<N + 1>(indices) - std::get<N>(indices))...);
        }
    }
}

template <bool UseRow, int... N, typename Derived, typename RowIndexContainer,
          typename ColIndexContainer>
auto _partition_outer_stride(std::integer_sequence<int, N...>,
                             const Eigen::MatrixBase<Derived>& M,
                             const RowIndexContainer& rows,
                             const ColIndexContainer& cols) {
    if constexpr (UseRow) {
        if constexpr (std::is_same_v<RowIndexContainer, types::empty_t>) {
            return _partition_inner_stride<false>(M, cols);
        } else {
            return std::make_tuple(_partition_inner_stride<false>(
                std::make_integer_sequence<
                    int, std::tuple_size_v<ColIndexContainer> - 1>{},
                M.block(std::get<N>(rows), 0,
                        std::get<N + 1>(rows) - std::get<N>(rows), M.cols()),
                cols)...);
        }
    } else {
        if constexpr (std::is_same_v<ColIndexContainer, types::empty_t>) {
            return _partition_inner_stride<true>(M, rows);
        } else {
            return std::make_tuple(_partition_inner_stride<true>(
                std::make_integer_sequence<
                    int, std::tuple_size_v<ColIndexContainer> - 1>{},
                M.block(0, std::get<N>(cols), M.rows(),
                        std::get<N + 1>(cols) - std::get<N>(cols)),
                rows)...);
        }
    }
}
}  // namespace detail
template <typename Derived, typename RowIndexContainer = mtao::types::empty_t,
          typename ColIndexContainer = mtao::types::empty_t>
auto partition_matrix(const Eigen::MatrixBase<Derived>& M,
                      const RowIndexContainer& rows,
                      const ColIndexContainer& cols) {
    // get the partition stride with row = true
    return detail::_partition_outer_stride<true>(
        std::make_integer_sequence<int,
                                   std::tuple_size_v<RowIndexContainer> - 1>(),
        M, rows, cols);
}

template <typename Derived, typename RowIndexContainer = mtao::types::empty_t,
          typename ColIndexContainer = mtao::types::empty_t>
auto partition_matrix(Eigen::PlainObjectBase<Derived>& M,
                      const RowIndexContainer& rows,
                      const ColIndexContainer& cols) {
    // get the partition stride with row = true
    return detail::_partition_outer_stride_writable<true>(
        std::make_integer_sequence<int,
                                   std::tuple_size_v<RowIndexContainer> - 1>(),
        M, rows, cols);
}

template <typename Derived, typename RowIndexContainer = mtao::types::empty_t,
          typename ColIndexContainer = mtao::types::empty_t>
auto partition_matrix_from_sizes(const Eigen::MatrixBase<Derived>& M,
                                 const RowIndexContainer& rows,
                                 const ColIndexContainer& cols) {
    // get the partition stride with row = true
    return partition_matrix(M, utils::partial_sum(rows),
                            utils::partial_sum(cols));
}

template <typename Derived, typename RowIndexContainer = mtao::types::empty_t,
          typename ColIndexContainer = mtao::types::empty_t>
auto partition_matrix_from_sizes(Eigen::PlainObjectBase<Derived>& M,
                                 const RowIndexContainer& rows,
                                 const ColIndexContainer& cols) {
    // get the partition stride with row = true
    return partition_matrix(M, utils::partial_sum(rows),
                            utils::partial_sum(cols));
}
}  // namespace mtao::eigen

