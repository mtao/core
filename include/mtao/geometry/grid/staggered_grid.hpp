#pragma once
#include "mtao/algebra/combinatorial.hpp"
#include <tuple>

namespace mtao {
    namespace geometry {
        namespace grid {
            namespace staggered_grid {
                namespace internal {
                    template <size_t E, size_t K, size_t L, size_t... N>
                        constexpr std::array<int,E> offset_shape(std::integer_sequence<size_t,N...>, const std::array<int,E>& shape) {
                            constexpr auto offsets = mtao::combinatorial::nCr_mask<E,K,L>();
                            return {{(shape[N]+offsets[N])...}};
                        }
                    template <size_t E, size_t... N>
                        constexpr std::array<int,E> offset_shape(std::integer_sequence<size_t,N...>, size_t K, size_t L, const std::array<int,E>& shape) {
                            constexpr auto offsets = mtao::combinatorial::nCr_mask<E>(K,L);
                            return {{(shape[N]+offsets[N])...}};
                        }

                    template <size_t E, size_t... N>
                        constexpr int offset_product(std::integer_sequence<size_t,N...>, const std::array<int,E>& shape, const std::bitset<E>& offsets) {
                            return ((shape[N]+offsets[N]) * ...);
                        }
                    template <size_t E>
                        constexpr int offset_product(const std::array<int,E>& shape, const std::bitset<E>& offsets) {
                            return offset_product(std::make_integer_sequence<size_t,E>(),shape,offsets);
                        }


                    template <size_t E, size_t D, size_t N>
                        constexpr int staggered_grid_size(const std::array<int,E>& shape) {
                            return offset_product(shape,mtao::combinatorial::nCr_mask<E,D,N>());
                        }

                    template <size_t E, size_t D, size_t... N>
                        constexpr auto staggered_grid_sizes_single_dim(std::integer_sequence<size_t,N...>, const std::array<int,E>& shape) {
                            return std::array<int,sizeof...(N)>{{staggered_grid_size<E,D,N>(shape)...}};
                        }
                    template <size_t D, size_t... N>
                        constexpr auto staggered_grid_sizes(std::integer_sequence<size_t,N...>, const std::array<int,D>& shape) {
                            return std::make_tuple(staggered_grid_sizes_single_dim<D,N>(std::make_integer_sequence<size_t,mtao::combinatorial::nCr(D,N)>(),shape)...);
                        }
                }


                //creates a tuple of tuples of grid sizes
                template <size_t D>
                    constexpr auto staggered_grid_sizes(const std::array<int,D>& shape) {
                        return internal::staggered_grid_sizes(std::make_integer_sequence<size_t,D+1>(),shape);
                    }

                //shape of a K-cube in a E-complex, on type L
                template <size_t E, size_t K, size_t L>
                    constexpr std::array<int,E> offset_shape(const std::array<int,E>& shape) {
                        return internal::offset_shape<E,K,L>(std::make_integer_sequence<int,E>(),shape);
                    }

                //shape of a K-cube in a E-complex, on type L
                template <size_t E>
                    constexpr std::array<int,E> offset_shape(size_t K, size_t L,const std::array<int,E>& shape) {
                        return internal::offset_shape<E>(std::make_integer_sequence<int,E>(),K,L,shape);
                    }
            }
        }
    }
}
