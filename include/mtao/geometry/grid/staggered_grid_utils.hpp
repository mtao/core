#pragma once
#include "mtao/algebra/combinatorial.hpp"
#include "mtao/type_utils.h"
#include <tuple>
#include <algorithm>
#include <numeric>

namespace mtao {
namespace geometry {
    namespace grid {
        namespace staggered_grid {
            namespace internal {
                template<size_t E, size_t K, size_t L, size_t... N>
                constexpr std::array<int, E> offset_shape(std::integer_sequence<size_t, N...>, const std::array<int, E> &shape) {
                    constexpr auto offsets = combinatorial::nCr_mask<E, K, L>();
                    return { { (shape[N] - offsets[N])... } };
                }

                template<size_t D, typename Func>
                void bitmask_looper(const std::bitset<D> &mask, Func &&f) {
                    for (int i = 0; i < (2 << (D - 1)); ++i) {
                        std::bitset<D> bs(i);
                        if ((bs & mask) == bs) {//if not in the span
                            f(bs);
                        }
                    }
                }
                template<size_t E, size_t... N>
                constexpr std::array<int, E> offset_shape(std::integer_sequence<size_t, N...>, size_t K, size_t L, const std::array<int, E> &shape) {
                    constexpr auto offsets = combinatorial::nCr_mask<E>(K, L);
                    return { { (shape[N] + offsets[N])... } };
                }

                template<size_t E, size_t K, size_t L>
                constexpr std::array<int, E> offset_shape(const std::array<int, E> &shape) {
                    return offset_shape<E, K, L>(std::make_integer_sequence<size_t, E>(), shape);
                }
                template<size_t E>
                constexpr std::array<int, E> offset_shape(size_t K, size_t L, const std::array<int, E> &shape) {
                    return offset_shape<E>(std::make_integer_sequence<size_t, E>(), K, L, shape);
                }

                template<size_t E, size_t... N>
                constexpr int offset_product(std::integer_sequence<size_t, N...>, const std::array<int, E> &shape, const std::bitset<E> &offsets) {
                    return ((shape[N] - offsets[N]) * ...);
                }
                template<size_t E>
                constexpr int offset_product(const std::array<int, E> &shape, const std::bitset<E> &offsets) {
                    return offset_product(std::make_integer_sequence<size_t, E>(), shape, offsets);
                }


                template<size_t E, size_t D, size_t N>
                constexpr int staggered_grid_size(const std::array<int, E> &shape) {
                    return offset_product(shape, combinatorial::nCr_mask<E, D, N>());
                }

                template<size_t E, size_t D, size_t... N>
                constexpr auto staggered_grid_sizes_single_dim(std::integer_sequence<size_t, N...>, const std::array<int, E> &shape) {
                    return std::array<int, sizeof...(N)>{ { staggered_grid_size<E, D, N>(shape)... } };
                }
                template<size_t D, size_t... N>
                constexpr auto staggered_grid_sizes(std::integer_sequence<size_t, N...>, const std::array<int, D> &shape) {
                    return std::make_tuple(staggered_grid_sizes_single_dim<D, N>(std::make_integer_sequence<size_t, combinatorial::nCr(D, N)>(), shape)...);
                }

                template<int N, int K, typename GridType>
                auto make_grid(const GridType &g) {
                    constexpr auto mask = combinatorial::nCr_mask<GridType::D, N, K>();
                    using Vec = typename GridType::Vec;
                    Vec offsets;
                    for (size_t i = 0; i < GridType::D; ++i) {
                        offsets(i) = .5 * mask[i];
                    }
                    auto new_shape = offset_shape<GridType::D, N, K>(g.shape());
                    return GridType(new_shape, g.dx(), g.origin() + g.dx().asDiagonal() * offsets);
                }
                template<int N, typename GridType, size_t... K>
                auto make_grids_single_dim(std::integer_sequence<size_t, K...>, const GridType &g) {
                    return std::array<GridType, sizeof...(K)>{ { make_grid<N, K>(g)... } };
                }
                template<typename GridType, size_t... N>
                auto make_grids(std::integer_sequence<size_t, N...>, const GridType &g) {
                    return std::make_tuple(make_grids_single_dim<N>(std::make_integer_sequence<size_t, combinatorial::nCr(GridType::D, N)>(), g)...);
                }

                template<typename SizeType, size_t... N>
                auto partial_sum_per_size(std::integer_sequence<size_t, N...>, const SizeType &sizes) {
                    auto sum = [](auto &&sizes) {
                        using T = std::decay_t<decltype(sizes)>;
                        constexpr size_t s = std::tuple_size<T>() + 1;
                        std::array<int, s> m{ {} };
                        m[0] = 0;
                        std::partial_sum(sizes.begin(), sizes.end(), m.begin() + 1);
                        return m;
                    };
                    return std::make_tuple(sum(std::get<N>(sizes))...);
                }
            }// namespace internal


            //creates a tuple of tuples of grid sizes
            template<size_t D>
            constexpr auto staggered_grid_sizes(const std::array<int, D> &shape) {
                return internal::staggered_grid_sizes(std::make_integer_sequence<size_t, D + 1>(), shape);
            }
            //creates a tuple of tuples of grid sizes
            template<size_t D>
            constexpr auto staggered_grid_offsets(const std::array<int, D> &shape) {
                auto sizes = staggered_grid_sizes(shape);
                return internal::partial_sum_per_size(std::make_integer_sequence<size_t, D + 1>(), sizes);
            }
            template<typename GridType>
            auto make_grids(const GridType &g) {
                return internal::make_grids(std::make_integer_sequence<size_t, GridType::D + 1>(), g);
            }

            //shape of a K-cube in a E-complex, on type L
            template<size_t E, size_t K, size_t L>
            constexpr std::array<int, E> offset_shape(const std::array<int, E> &shape) {
                return internal::offset_shape<E, K, L>(std::make_integer_sequence<int, E>(), shape);
            }

            //shape of a K-cube in a E-complex, on type L
            template<size_t E>
            constexpr std::array<int, E> offset_shape(size_t K, size_t L, const std::array<int, E> &shape) {
                return internal::offset_shape<E>(std::make_integer_sequence<int, E>(), K, L, shape);
            }

        }// namespace staggered_grid
    }// namespace grid
}// namespace geometry
}// namespace mtao
