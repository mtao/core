#pragma once
#include "mtao/algebra/combinatorial.hpp"
#include <tuple>
#include <algorithm>

namespace mtao {
    namespace geometry {
        namespace grid {
            namespace staggered_grid {
                namespace internal {
                    template <size_t E, size_t K, size_t L, typename UseVertexTag=std::false_type, size_t... N>
                        constexpr std::array<int,E> offset_shape(std::integer_sequence<size_t,N...>, const std::array<int,E>& shape, UseVertexTag={}) {
                            constexpr auto offsets = combinatorial::nCr_mask<E,K,L>();
                            if constexpr(UseVertexTag::value) {
                                return {{(shape[N]-offsets[N])...}};
                            } else {
                                return {{(shape[N]+(1-offsets[N]))...}};
                            }
                        }
                    template <size_t E, size_t... N, typename UseVertexTag=std::false_type>
                        constexpr std::array<int,E> offset_shape(std::integer_sequence<size_t,N...>, size_t K, size_t L, const std::array<int,E>& shape, UseVertexTag={}) {
                            constexpr auto offsets = combinatorial::nCr_mask<E>(K,L,UseVertexTag());
                            return {{(shape[N]+offsets[N])...}};
                        }

                    template <size_t E, size_t K, size_t L, typename UseVertexTag=std::false_type>
                        constexpr std::array<int,E> offset_shape(const std::array<int,E>& shape, UseVertexTag={}) {
                            return offset_shape<E,K,L>(std::make_integer_sequence<size_t,E>(),shape,UseVertexTag());
                        }
                    template <size_t E, typename UseVertexTag=std::false_type>
                        constexpr std::array<int,E> offset_shape(size_t K, size_t L, const std::array<int,E>& shape, UseVertexTag={}) {
                            return offset_shape<E>(std::make_integer_sequence<size_t,E>(),K,L,shape,UseVertexTag());
                        }

                    template <size_t E, size_t... N, typename UseVertexTag=std::false_type>
                        constexpr int offset_product(std::integer_sequence<size_t,N...>, const std::array<int,E>& shape, const std::bitset<E>& offsets, UseVertexTag={}) {
                            if constexpr(UseVertexTag::value) {
                                return ((shape[N]-offsets[N]) * ...);
                            } else {
                                return ((shape[N]+(1-offsets[N])) * ...);
                            }
                        }
                    template <size_t E, typename UseVertexTag=std::false_type>
                        constexpr int offset_product(const std::array<int,E>& shape, const std::bitset<E>& offsets, UseVertexTag={}) {
                            return offset_product(std::make_integer_sequence<size_t,E>(),shape,offsets, UseVertexTag());
                        }


                    template <size_t E, size_t D, size_t N, typename UseVertexTag=std::false_type>
                        constexpr int staggered_grid_size(const std::array<int,E>& shape, UseVertexTag={}) {
                            return offset_product(shape,combinatorial::nCr_mask<E,D,N>(), UseVertexTag());
                        }

                    template <size_t E, size_t D, size_t... N, typename UseVertexTag=std::false_type>
                        constexpr auto staggered_grid_sizes_single_dim(std::integer_sequence<size_t,N...>, const std::array<int,E>& shape, UseVertexTag={}) {
                            return std::array<int,sizeof...(N)>{{staggered_grid_size<E,D,N>(shape, UseVertexTag())...}};
                        }
                    template <size_t D, size_t... N, typename UseVertexTag=std::false_type >
                        constexpr auto staggered_grid_sizes(std::integer_sequence<size_t,N...>, const std::array<int,D>& shape) {
                            return std::make_tuple(staggered_grid_sizes_single_dim<D,N>(std::make_integer_sequence<size_t,combinatorial::nCr(D,N)>(),shape)...);
                        }

                    template <int N, int K, typename GridType, typename UseVertexTag=std::false_type>
                    auto make_grid(const GridType& g,UseVertexTag={}) {
                        constexpr auto mask = combinatorial::nCr_mask<GridType::D,N,K>();
                        using Vec = typename GridType::Vec;
                        Vec offsets;
                        for(size_t i = 0; i < GridType::D; ++i) {
                            offsets(i) = .5 * mask[i];
                        }
                        auto new_shape = offset_shape<GridType::D,N,K>(g.shape(),UseVertexTag());
                        return GridType(new_shape,g.dx(),g.origin() + g.dx().asDiagonal() * offsets);
                    }
                template <int N, typename GridType, size_t...K, typename UseVertexTag=std::false_type>
                    auto make_grids_single_dim(std::integer_sequence<size_t,K...>, const GridType& g,UseVertexTag={}) {
                            return std::array<GridType,sizeof...(K)>{{make_grid<N,K>(g,UseVertexTag())...}};
                    }
                template <typename GridType, size_t...N, typename UseVertexTag=std::false_type>
                    auto make_grids(std::integer_sequence<size_t,N...>, const GridType& g,UseVertexTag={}) {
                            return std::make_tuple(make_grids_single_dim<N>(std::make_integer_sequence<size_t,combinatorial::nCr(GridType::D,N)>(),g, UseVertexTag())...);
                    }
                }


                //creates a tuple of tuples of grid sizes
                template <size_t D, typename UseVertexTag=std::false_type>
                    constexpr auto staggered_grid_sizes(const std::array<int,D>& shape, UseVertexTag={}) {
                        return internal::staggered_grid_sizes(std::make_integer_sequence<size_t,D+1>(),shape);
                    }
                template <typename GridType, typename UseVertexTag=std::false_type>
                    auto make_grids(const GridType& g,UseVertexTag={}) {
                        return internal::make_grids(std::make_integer_sequence<size_t,GridType::D+1>(),g, UseVertexTag());
                    }

                //shape of a K-cube in a E-complex, on type L
                template <size_t E, size_t K, size_t L, typename UseVertexTag=std::false_type>
                    constexpr std::array<int,E> offset_shape(const std::array<int,E>& shape, UseVertexTag={}) {
                        return internal::offset_shape<E,K,L>(std::make_integer_sequence<int,E>(),shape, UseVertexTag());
                    }

                //shape of a K-cube in a E-complex, on type L
                template <size_t E, typename UseVertexTag=std::false_type>
                    constexpr std::array<int,E> offset_shape(size_t K, size_t L,const std::array<int,E>& shape, UseVertexTag={}) {
                        return internal::offset_shape<E>(std::make_integer_sequence<int,E>(),K,L,shape, UseVertexTag());
                    }

            }
        }
    }
}
