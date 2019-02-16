#pragma once
#include "mtao/algebra/combinatorial.hpp"
#include "staggered_grid_utils.hpp"
#include "mtao/iterator/enumerate.hpp"
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
                        using Base::dx;
                        using Base::shape;
                        using Base::size;
                        using Base::index;
                        using Base::unindex;
                        using coord_type = typename Base::coord_type;
                        using Vec = typename Base::Vec;
                        using VecMap = typename Base::VecMap;
                        using CVecMap = typename Base::CVecMap;
                        using IVec = typename Base::IVec;
                        using IVecMap = typename Base::IVecMap;
                        using CIVecMap = typename Base::CIVecMap;
                        using StaggeredGrids = decltype(staggered_grid::make_grids(std::declval<Base>()));
                        /*
                        StaggeredGrid(const coord_type& shape): Base(shape) {
                            resize_grids();
                        }
                        */
                        template <typename... Args>
                            StaggeredGrid(Args&&... args): Base(std::forward<Args>(args)...) {
                                resize_grids();
                            }
                        //template <typename... Args>
                        //    StaggeredGrid(const coord_type& shape, Args&&... args): Base(shape, std::forward<Args>(args)...) {
                        //        resize_grids();
                        //    }
                        StaggeredGrid() {}
                        StaggeredGrid(const StaggeredGrid& other) = default;
                        StaggeredGrid(StaggeredGrid&& other) = default;
                        StaggeredGrid& operator=(const StaggeredGrid& other) = default;
                        StaggeredGrid& operator=(StaggeredGrid&& other) = default;

                        auto&& origin() const { return vertex_grid().origin(); }
                        template <int N, int K>
                            const GridType& grid() const {
                            return std::get<K>(std::get<N>(m_grids));
                            }
                        template <int N>
                            auto&& grids() const {
                            return std::get<N>(m_grids);
                            }
                        template <int N>
                            const GridType& grid(int K) const {
                            return std::get<N>(m_grids)[K];
                            }
                        auto&& vertex_grid() const {
                            return grid<0,0>();
                        }
                        auto&& cell_grid() const {
                            return grid<D,0>();
                        }
                        template <int N>
                            auto offsets() const {
                                return std::get<N>(m_offsets);
                            }
                        template <int N, int K>
                            int offset() const {
                                return offsets<N>()[K];
                            }
                        template <int N>
                            int offset(int K) const {
                                return offsets<N>()[K];
                            }


                        template <int N, int K> size_t staggered_index(const coord_type& idx) const {return offset<N,K>() + grid<N,K>().index(idx);}
                        template <int N> size_t staggered_index(const coord_type& idx, int K) const {return offset<N>(K) + grid<N>(K).index(idx);}
                        template <int N, int K> auto staggered_unindex(int idx) const {return grid<N,K>().unindex(idx - offset<N,K>());}
                        template <int N> auto staggered_unindex(int idx, int K) const {return grid<N>(K).unindex(idx - offset<N>(K));}
                        template <int N, int K> auto staggered_vertices() const {return grid<N,K>().vertices();}
                        template <int N, int K> auto staggered_vertex(const coord_type& idx) const {return grid<N,K>().vertex(idx);}
                        template <int N, int K> const coord_type& staggered_shape() const {return grid<N,K>().shape();}
                        template <int N, int K> size_t staggered_size() const {return grid<N,K>().size();}
                        template <int N> auto staggered_vertices(int K) const {return grid<N>(K).vertices();}
                        template <int N> auto staggered_vertex(const coord_type& idx, int K) const {return grid<N>(K).vertex(idx);}
                        template <int N> const coord_type& staggered_shape(int K) const {return grid<N>(K).shape();}
                        template <int N> size_t staggered_size(int K) const {return grid<N>(K).size();}
                        template <int N, int K=0>
                        auto staggered_vertex(int idx) const {return staggered_vertex<N,K>(staggered_unindex<N,K>(idx));}
                        template <int N>
                        auto staggered_vertex(int idx, int K) const {return staggered_vertex<N>(staggered_unindex<N>(idx,K),K);}



                        auto vertex(const coord_type& idx) const {return staggered_vertex<0,0>(idx);}
                        auto vertex(int idx) const {return vertex(staggered_unindex<0,0>(idx));}
                        auto vertices() const {return staggered_vertices<0,0>();}
                        auto cell_vertex(const coord_type& idx) const {return staggered_vertex<D>(idx);}
                        auto cell_vertex(int idx) const {return vertex(staggered_unindex<D,0>(idx));}
                        auto cell_vertices() const {return staggered_vertices<D,0>();}


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

                        template <int D>
                        size_t form_size() const {
                            auto&& gs = std::get<D>(m_offsets);
                            using U = types::remove_cvref_t<decltype(gs)>;
                            return gs[std::tuple_size<U>()-1];
                        }
                        size_t edge_size() const {return form_size<1>();}
                        size_t flux_size() const {return form_size<D-1>();}

                        template <int D>
                        int form_type(int index) const {
                            if(index < 0) {
                                return -1;
                            }
                            using namespace iterator;
                            auto&& ofs = offsets<D>();
                            size_t result = 0;
                            using U = types::remove_cvref_t<decltype(ofs)>;
                            for(auto&& [i,v]: enumerate(reverse(ofs))) {
                                if(index >= v) {
                                    result = std::tuple_size<U>() - i - 1;
                                    break;
                                }
                            }
                            if(result >= ofs.size()) {
                                return -1;
                            }
                            return result;
                        }

                        size_t edge_type(int index) const {
                            return form_type<1>(index);
                        }
                        template <int D>
                            auto form_unindex(int idx) const {
                                int ft = form_type<D>(idx);
                                return std::make_tuple(staggered_unindex<D>(idx,ft),ft);

                            }
                        template <typename Derived>
                        auto coord(const Eigen::MatrixBase<Derived>& v) const {

                            return vertex_grid().coord(v);
                        }

                    private:
                        void resize_grids() {
                            m_grids = staggered_grid::make_grids(*static_cast<Base*>(this),std::integral_constant<bool,UseVertexGrid>());
                            m_offsets = staggered_grid::staggered_grid_offsets(shape(),std::integral_constant<bool,UseVertexGrid>());
                        }
                        StaggeredGrids m_grids;
                        decltype(staggered_grid::staggered_grid_offsets(std::declval<coord_type>())) m_offsets;
                        

                };

        }
    }
}
