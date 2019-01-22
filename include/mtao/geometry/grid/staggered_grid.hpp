#pragma once
#include "mtao/algebra/combinatorial.hpp"
#include "staggered_grid_utils.hpp"
#include "grid.h"
#include <tuple>

namespace mtao {
    namespace geometry {
        namespace grid {

            template <typename T, int Dim, bool UseVertexGrid=false>
                struct StaggeredGrid: public GridD<T,Dim, UseVertexGrid> {
                   
                    public:
                        constexpr static int D = Dim;
                        using GridType = GridD<T,Dim, UseVertexGrid>;
                        using Base = GridType;
                        using coord_type = typename Base::coord_type;
                        using StaggeredGrids = decltype(staggered_grid::make_grids(std::declval<Base>()));
                        template <typename... Args> 
                            StaggeredGrid(const coord_type& shape, Args&&... args): Base(shape, std::forward<Args>(args)...) {
                                resize_grids();
                            }
                        StaggeredGrid() {}
                        StaggeredGrid(const StaggeredGrid& other) = default;
                        StaggeredGrid(StaggeredGrid&& other) = default;
                        StaggeredGrid& operator=(const StaggeredGrid& other) = default;
                        StaggeredGrid& operator=(StaggeredGrid&& other) = default;
                        template <int N, int K>
                            const GridType& grid() const {
                            return std::get<K>(std::get<N>(m_grids));
                            }
                        template <int N, int K> size_t staggered_index(const coord_type& idx) const {return grid<N,K>().index(idx);}
                        template <int N, int K> auto staggered_vertices() const {return grid<N,K>().vertices();}
                        template <int N, int K> auto staggered_vertex(const coord_type& idx) const {return grid<N,K>().vertex(idx);}
                        template <int N, int K> const coord_type& staggered_shape() const {return grid<N,K>().shape();}
                        template <int N, int K> size_t staggered_size() const {return grid<N,K>().size();}

                        auto vertex(const coord_type& idx) const {return staggered_vertex<0,0>(idx);}
                        auto vertices() const {return staggered_vertices<0,0>();}


                        size_t u_index(const coord_type& idx) const { return staggered_index<1,0>(idx);}
                        size_t v_index(const coord_type& idx) const { return staggered_index<1,1>(idx);}
                        size_t w_index(const coord_type& idx) const { return staggered_index<1,2>(idx);}
                        size_t vw_index(const coord_type& idx) const { return staggered_index<2,0>(idx);}
                        size_t uw_index(const coord_type& idx) const { return staggered_index<2,1>(idx);}
                        size_t uv_index(const coord_type& idx) const { return staggered_index<2,2>(idx);}
                        size_t vertex_index(const coord_type& idx) const { return staggered_index<0,0>(idx);}
                        size_t cell_index(const coord_type& idx) const { return staggered_index<D,0>(idx);}

                        const coord_type& u_shape() const { return staggered_shape<1,0>();}
                        const coord_type& v_shape() const { return staggered_shape<1,1>();}
                        const coord_type& w_shape() const { return staggered_shape<1,2>();}
                        const coord_type& vw_shape() const { return staggered_shape<2,0>();}
                        const coord_type& uw_shape() const { return staggered_shape<2,1>();}
                        const coord_type& uv_shape() const { return staggered_shape<2,2>();}
                        const coord_type& vertex_shape() const { return staggered_shape<0,0>();}
                        const coord_type& cell_shape() const { return staggered_shape<D,0>();}

                        size_t u_size() const { return staggered_size<1,0>();}
                        size_t v_size() const { return staggered_size<1,1>();}
                        size_t w_size() const { return staggered_size<1,2>();}
                        size_t vw_size() const { return staggered_size<2,0>();}
                        size_t uw_size() const { return staggered_size<2,1>();}
                        size_t uv_size() const { return staggered_size<2,2>();}
                        size_t vertex_size() const { return staggered_size<0,0>();}
                        size_t cell_size() const { return staggered_size<D,0>();}


                    private:
                        void resize_grids() {
                            m_grids = staggered_grid::make_grids(*static_cast<Base*>(this),std::integral_constant<bool,UseVertexGrid>());
                        }
                        StaggeredGrids m_grids;
                        

                };

        }
    }
}
