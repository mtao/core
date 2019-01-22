#include "mtao/geometry/grid/grid_utils.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#include <mtao/logging/timer.hpp>



namespace mtao { namespace geometry { namespace grid {

    template <typename GridType>
        ColVectors<unsigned int, 3> GridTriangulator<GridType>::faces() const {

        auto t = logging::timer("face creation");
            if constexpr(D == 2) {
                auto [i,j] = g.shape();
                ColVectors<unsigned int,3> F(3,2*(i-1)*(j-1));
                int counter = 0;
                auto idx = [&](int ii, int jj) -> size_t { return ii + i * jj; };
                utils::multi_loop(std::array<size_t,2>{{i-1,j-1}},[&](auto&& ij) {
                        auto [ii,jj] = ij;
                        std::array<size_t,4> x{{idx(ii,jj),idx(ii+1,jj),idx(ii,jj+1),idx(ii+1,jj+1)}};
                        //std::array<size_t,4> x{{g.index(ii,jj),g.index(ii+1,jj),g.index(ii,jj+1),g.index(ii+1,jj+1)}};
                        F.col(counter++) = Vector<unsigned int,3>(x[0],x[1],x[2]);
                        F.col(counter++) = Vector<unsigned int,3>(x[1],x[2],x[3]);
                        });
                return F;
            } else if constexpr(D == 3) {
                auto [i,j,k] = g.shape();
                ColVectors<unsigned int,3> F(3,
                        2*((i)*(j-1)*(k-1)
                            +(i-1)*(j)*(k-1)
                            +(i-1)*(j-1)*(k))
                        );
                int counter = 0;
                utils::multi_loop(std::array<size_t,3>{{i,j-1,k-1}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        std::array<size_t,4> x{{g.index(ii,jj,kk),g.index(ii,jj+1,kk),g.index(ii,jj,kk+1),g.index(ii,jj+1,kk+1)}};
                        F.col(counter++) = Vector<unsigned int,3>(x[0],x[1],x[2]);
                        F.col(counter++) = Vector<unsigned int,3>(x[1],x[2],x[3]);
                        });
                utils::multi_loop(std::array<size_t,3>{{i-1,j,k-1}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        std::array<size_t,4> x{{g.index(ii,jj,kk),g.index(ii+1,jj,kk),g.index(ii,jj,kk+1),g.index(ii+1,jj,kk+1)}};
                        F.col(counter++) = Vector<unsigned int,3>(x[0],x[1],x[2]);
                        F.col(counter++) = Vector<unsigned int,3>(x[1],x[2],x[3]);
                        });
                utils::multi_loop(std::array<size_t,3>{{i-1,j-1,k}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        std::array<size_t,4> x{{g.index(ii,jj,kk),g.index(ii+1,jj,kk),g.index(ii,jj+1,kk),g.index(ii+1,jj+1,kk)}};
                        F.col(counter++) = Vector<unsigned int,3>(x[0],x[1],x[2]);
                        F.col(counter++) = Vector<unsigned int,3>(x[1],x[2],x[3]);
                        });
                return F;
            }
        }
    template <typename GridType>
        ColVectors<unsigned int, 2> GridTriangulator<GridType>::edges() const {
            if constexpr( D == 2 ) {
                auto [i,j] = g.shape();
                ColVectors<unsigned int,2> E(2,
                        (i-1)*(j)
                        +(i)*(j-1)
                        );

                int counter = 0;
                utils::multi_loop(std::array<size_t,2>{{i-1,j}},[&](auto&& ij) {
                        auto [ii,jj] = ij;
                        unsigned int o = index(ii,jj);
                        unsigned int x = index(ii+1,jj);
                        E.col(counter++) = Vector<unsigned int,2>(o,x);
                        });
                utils::multi_loop(std::array<size_t,2>{{i,j-1}},[&](auto&& ij) {
                        auto [ii,jj] = ij;
                        unsigned int o = index(ii,jj);
                        unsigned int y = index(ii,jj+1);
                        E.col(counter++) = Vector<unsigned int,2>(o,y);
                        });
                utils::multi_loop(std::array<size_t,2>{{i,j}},[&](auto&& ij) {
                        auto [ii,jj] = ij;
                        unsigned int o = index(ii,jj);
                        unsigned int z = index(ii,jj+1);
                        E.col(counter++) = Vector<unsigned int,2>(o,z);
                        });
                return E;
            } else if constexpr( D == 3) {
                auto [i,j,k] = g.shape();
                ColVectors<unsigned int,2> E(2,
                        (i-1)*(j)*(k)
                        +(i)*(j-1)*(k)
                        +(i)*(j)*(k-1)
                        );

                int counter = 0;
                utils::multi_loop(std::array<size_t,3>{{i-1,j,k}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        unsigned int o = g.index(ii,jj,kk);
                        unsigned int x = g.index(ii+1,jj,kk);
                        E.col(counter++) = Vector<unsigned int,2>(o,x);
                        });
                utils::multi_loop(std::array<size_t,3>{{i,j-1,k}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        unsigned int o = g.index(ii,jj,kk);
                        unsigned int y = g.index(ii,jj+1,kk);
                        E.col(counter++) = Vector<unsigned int,2>(o,y);
                        });
                utils::multi_loop(std::array<size_t,3>{{i,j,k-1}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        unsigned int o = g.index(ii,jj,kk);
                        unsigned int z = g.index(ii,jj,kk+1);
                        E.col(counter++) = Vector<unsigned int,2>(o,z);
                        });
                return E;
            }
        }
    template <typename GridType>
        auto  GridTriangulator<GridType>::vertices() const -> ColVectors<T,D> {
        auto t = logging::timer("vertex creation");
            return g.vertices();
        }
}}}
#pragma GCC diagnostic pop
