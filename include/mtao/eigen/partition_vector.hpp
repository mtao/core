#pragma once
#include "mtao/eigen/partition_matrix.hpp"
namespace mtao::eigen {

// for vector-like cases it simply generates
// {a,b} for either
// [ A B ]
// or
// [ A ]
// [ B ]
// the default variant takes the offsets, presumably starting with 0 and the
// size This is equivalent to setting the boundary of partitions, i.e indices
// {0,1,5} generates 2 partitions from [0,1) and [1,5).
//
// the *_from_sizes variants take the sizes of the blocks themselves
//
//
// Example usage:
// When solving pressure as a KKT problem with velocities, pressures, and constraints one obtains
// [ M 0   Dx^T Cx^T ] [Vx]     = [vx]
// [ 0 M   Dy^T Cy^T ] [Vy]     = [vy]
// [ Dx Dy 0    Cp^T ] [p]      = [0]
// [ Cx Cy Cp   0    ] [lambda] = [c]
// - partition_matrix can be used to generate dense versions of the KKT matrix 
/*
int num_vx;// number of x coordinate samples
int num_vy;// number of y coordinate samples
int num_p;// number of pressure samples
int num_c;// number of constraints

tuple nums(num_vx,num_vy,num_p,num_c)


Eigen::VectorXd rhs(num_vx+num_vy+num_p+num_c);
// we can unpack each block using a structured binding
auto [rhsx,rhsy,rhsp,rhsl] = partition_vector_from_sizes(rhs,nums);
rhsx = vx;
rhsy = vy;
rhsp.setZero();
rhsc = c;
// alternatively we could have done 
partition_vector_from_sizes(rhs,nums) = tuple(vx,vy,Eigen::VectorXd::Zero(num_p),c);

Eigen::VectorXd x = solve(A,rhs);
// finally we can write write directly
std::tie(Vx,Vy,p,lambda) = partition_vector_from_sizes(x,nums);
*/

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
template <typename Derived, typename IndexContainer>
auto partition_vector_from_sizes(const Eigen::MatrixBase<Derived>& M,
                                 const IndexContainer& indices) {
    return partition_vector(M, utils::partial_sum(indices));
}
template <typename Derived, typename IndexContainer>
auto partition_vector_from_sizes(Eigen::PlainObjectBase<Derived>& M,
                                 const IndexContainer& indices) {
    return partition_vector(M, utils::partial_sum(indices));
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
