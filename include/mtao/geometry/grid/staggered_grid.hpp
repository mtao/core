#pragma once
#include "mtao/algebra/combinatorial.hpp"
#include "staggered_grid_utils.hpp"
#include <Eigen/Sparse>
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
                        using Scalar = T;
                        using BBox = typename Base::BBox;
                        using Indexer= typename Base::Indexer;
                        using coord_type = typename Base::coord_type;
                        using Vec = typename Base::Vec;
                        using VecMap = typename Base::VecMap;
                        using CVecMap = typename Base::CVecMap;
                        using IVec = typename Base::IVec;
                        using IVecMap = typename Base::IVecMap;
                        using CIVecMap = typename Base::CIVecMap;
                        using StaggeredGrids = decltype(staggered_grid::make_grids(std::declval<Base>()));

                        template <int N>
                        constexpr static size_t form_grid_size() {
                            return std::tuple_size<std::tuple_element_t<N,StaggeredGrids>>();
                        }
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

                        auto bbox() const {
                            return vertex_grid().bbox();
                        }
                        static StaggeredGrid from_bbox(BBox bb, const coord_type& shape, bool cubes = false) {

                            Vec mid =(bb.min() + bb.max()) / 2;
                            auto dx = (bb.sizes().array() / (CIVecMap(shape.data()).template cast<T>().array() + (UseVertexGrid?T(1):T(0)))).eval();
                            if(cubes) {
                                dx.setConstant(dx.maxCoeff());
                                bb.max() = (dx.array() * (CIVecMap(shape.data()).template cast<T>().array()+(UseVertexGrid?T(1):T(0)))).eval() / 2;
                                bb.min() = -bb.max(); 
                                bb.min() += mid;
                                bb.max() += mid;
                            }
                            auto o = bb.min();
                            return StaggeredGrid(shape,dx.matrix(),o);
                        }
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
                        template <int N, int K> bool staggered_valid_index(const coord_type & idx) const {return grid<N,K>().valid_index(idx);}
                        template <int N> bool staggered_valid_index(const coord_type & idx, int K) const {return grid<N>(K).valid_index(idx);}



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
                        coord_type u_unindex(size_t idx) const { return staggered_unindex<1,0>(idx);}
                        coord_type v_unindex(size_t idx) const { return staggered_unindex<1,1>(idx);}
                        coord_type w_unindex(size_t idx) const { return staggered_unindex<1,2>(idx);}
                        coord_type vw_unindex(size_t idx) const { return staggered_unindex<2,0>(idx);}
                        coord_type uw_unindex(size_t idx) const { return staggered_unindex<2,1>(idx);}
                        coord_type uv_unindex(size_t idx) const { return staggered_unindex<2,2>(idx);}
                        coord_type vertex_unindex(size_t idx) const { return staggered_unindex<0,0>(idx);}
                        coord_type cell_unindex(size_t idx) const { return staggered_unindex<D,0>(idx);}

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

                        bool u_valid_index(const coord_type& idx) const { return staggered_valid_index<1,0>(idx);}
                        bool v_valid_index(const coord_type& idx) const { return staggered_valid_index<1,1>(idx);}
                        bool w_valid_index(const coord_type& idx) const { return staggered_valid_index<1,2>(idx);}
                        bool vw_valid_index(const coord_type& idx) const { return staggered_valid_index<2,0>(idx);}
                        bool uw_valid_index(const coord_type& idx) const { return staggered_valid_index<2,1>(idx);}
                        bool uv_valid_index(const coord_type& idx) const { return staggered_valid_index<2,2>(idx);}
                        bool vertex_valid_index(const coord_type& idx) const { return staggered_valid_index<0,0>(idx);}
                        bool cell_valid_index(const coord_type& idx) const { return staggered_valid_index<D,0>(idx);}

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
                        template <int N>
                        Eigen::SparseMatrix<T> boundary() const {
                            int rows = form_size<N-1>();
                            int cols = form_size<N>();
                            std::vector<Eigen::Triplet<T>> trips;
                            for(auto&& [K,grids]: iterator::enumerate(std::get<N>(m_grids))) {
                                std::bitset<D> difference_mask = combinatorial::nCr_mask<D>(N,K);
                                grid<N>(K).loop([&,K=K](const coord_type& c) {
                                        int col = staggered_index<N>(c,K);
                                        auto s = staggered_shape<N>(K);
                                        masked_difference_looper(difference_mask,c,
                                                [&](const coord_type& l, const coord_type& u, int d) {
                                                std::bitset mybs = difference_mask;
                                                mybs[d] = 0;
                                                int dim = combinatorial::nCr_unmask<D>(N-1,mybs);

                                                auto s = staggered_shape<N-1>(dim);
                                                int lrow = static_cast<int>(staggered_index<N-1>(l,dim));
                                                int urow = static_cast<int>(staggered_index<N-1>(u,dim));
                                                trips.emplace_back(lrow,col,T{-1});
                                                trips.emplace_back(urow,col,T{1});
                                                });
                                        });
                            }

                            Eigen::SparseMatrix<T> A(rows,cols);
                            A.setFromTriplets(trips.begin(),trips.end());
                            return A;
                        }
                        template <int N, typename Func>
                            static void cell_vertex_looper(int K, const coord_type& c, Func&& f) {
                                staggered_grid::internal::bitmask_looper(combinatorial::nCr_mask<D>(N,K),[&](const std::bitset<D>& bs) {
                                        coord_type cc = c;
                                        for(int j = 0; j < D; ++j) {
                                        cc[j] += bs[j];
                                        }
                                        f(cc);

                                        });

                            }
                        template <int N, typename Func>
                            static void cell_dual_vertex_looper(int K, const coord_type& c, Func&& f) {
                                staggered_grid::internal::bitmask_looper(!combinatorial::nCr_mask<D>(N,K),[&](const std::bitset<D>& bs) {
                                        coord_type cc = c;
                                        for(int j = 0; j < D; ++j) {
                                        cc[j] -= bs[j];
                                        }
                                        f(cc);

                                        });

                            }

                        template <typename Func>
                            static void masked_difference_looper(const std::bitset<D>& mask, const coord_type& c, Func&& f) {
                                coord_type cc = c;
                                for(int j = 0; j < D; ++j) {

                                    if(mask[j]) {
                                        cc[j]++;
                                        const coord_type& ccc = cc;
                                        f(c,ccc, j);
                                        cc[j]--;
                                    }
                                }
                            }
                        //f( lower_index_boundary, higher_index_boundary, index) as a centered difference stencil
                        //for <1>::instance boundary_cell_looper<1>(0,{N+.5},f) calls
                        //          f({N},{N+1},0)
                        //for <2>::instance boundary_cell_looper<2>(1,{N+.5,M+.5},f) calls
                        //          f({N,M+.5},{N+1,M+.5},0)
                        //          f({N+.5,M},{N+.5,M+1},1)
                        template <int N, typename Func>
                            static void cell_boundary_looper(int K, const coord_type& c, Func&& f) {
                                auto comb = combinatorial::nCr_mask<D>(N,K);
                                masked_difference_looper(comb,c,f);
                            }

                        //opposite of the above
                        template <int N, typename Func>
                            static void dual_cell_boundary_looper(int K, const coord_type& c, Func&& f) {

                                coord_type cc = c;
                                auto comb = !combinatorial::nCr_mask<D>(N,K);
                                for(int j = 0; j < D; ++j) {
                                    if(comb[j]) {
                                        cc[j]--;
                                        const coord_type& ccc = cc;
                                        f(ccc,c, j);
                                        cc[j]++;
                                    }
                                }
                            }

                        template <int N>
                            auto form_vertices(int K, const coord_type& c) const {

                                std::array<int,1<<N> ret;
                                auto it = ret.begin();
                                cell_vertex_looper<N>(K,c,[&](const coord_type& c) {
                                        *(it++) = vertex_index(c);
                                        });
                                return ret;
                            }
                        template <int D>
                            auto form_vertices(int idx) const {
                                int K = form_type<D>(idx);
                                auto coord = staggered_unindex<D>(idx,K);
                                return form_vertices<D>(K,coord);
                            }
                        auto edge(int K, const coord_type& c) const -> std::array<int,2> {
                            return form_vertices<1>(K,c);
                        }
                        auto edge(int idx) const {
                            return form_vertices<1>(idx);
                        }

                    private:
                        void resize_grids() {
                            m_grids = staggered_grid::make_grids(*static_cast<Base*>(this),std::integral_constant<bool,UseVertexGrid>());
                            m_offsets = staggered_grid::staggered_grid_offsets(shape(),std::integral_constant<bool,UseVertexGrid>());
                        }
                        StaggeredGrids m_grids;
                        decltype(staggered_grid::staggered_grid_offsets(std::declval<coord_type>())) m_offsets;

                };

            using StaggeredGrid2f = StaggeredGrid<float,2>;
            using StaggeredGrid3f = StaggeredGrid<float,3>;
            using StaggeredGrid2d = StaggeredGrid<double,2>;
            using StaggeredGrid3d = StaggeredGrid<double,3>;
        }
    }
}
