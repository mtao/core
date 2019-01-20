#include "mtao/geometry/grid/grid_utils.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"



namespace mtao { namespace geometry { namespace grid {

    template <typename Indexer>
        ColVectors<unsigned int, 3> GridTopologicalTriangulation<Indexer>::faces() const {

            if constexpr(D == 2) {
                auto [i,j] = shape();
                ColVectors<unsigned int,3> F(3,2*(i-1)*(j-1));
                int counter = 0;
                utils::multi_loop(std::array<size_t,2>{{i-1,j-1}},[&](auto&& ij) {
                        auto [ii,jj] = ij;
                        std::array<size_t,4> x{{this->index(ii,jj),this->index(ii+1,jj),this->index(ii,jj+1),this->index(ii+1,jj+1)}};
                        F.col(counter++) = Vector<unsigned int,3>(x[0],x[1],x[2]);
                        F.col(counter++) = Vector<unsigned int,3>(x[1],x[2],x[3]);
                        });
                return F;
            } else if constexpr(D == 3) {
                auto [i,j,k] = shape();
                ColVectors<unsigned int,3> F(3,
                        2*((i)*(j-1)*(k-1)
                            +(i-1)*(j)*(k-1)
                            +(i-1)*(j-1)*(k))
                        );
                int counter = 0;
                utils::multi_loop(std::array<size_t,3>{{i,j-1,k-1}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        std::array<size_t,4> x{{this->index(ii,jj,kk),this->index(ii,jj+1,kk),this->index(ii,jj,kk+1),this->index(ii,jj+1,kk+1)}};
                        F.col(counter++) = Vector<unsigned int,3>(x[0],x[1],x[2]);
                        F.col(counter++) = Vector<unsigned int,3>(x[1],x[2],x[3]);
                        });
                utils::multi_loop(std::array<size_t,3>{{i-1,j,k-1}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        std::array<size_t,4> x{{this->index(ii,jj,kk),this->index(ii+1,jj,kk),this->index(ii,jj,kk+1),this->index(ii+1,jj,kk+1)}};
                        F.col(counter++) = Vector<unsigned int,3>(x[0],x[1],x[2]);
                        F.col(counter++) = Vector<unsigned int,3>(x[1],x[2],x[3]);
                        });
                utils::multi_loop(std::array<size_t,3>{{i-1,j-1,k}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        std::array<size_t,4> x{{this->index(ii,jj,kk),this->index(ii+1,jj,kk),this->index(ii,jj+1,kk),this->index(ii+1,jj+1,kk)}};
                        F.col(counter++) = Vector<unsigned int,3>(x[0],x[1],x[2]);
                        F.col(counter++) = Vector<unsigned int,3>(x[1],x[2],x[3]);
                        });
                return F;
            }
        }
    template <typename Indexer>
        ColVectors<unsigned int, 2> GridTopologicalTriangulation<Indexer>::edges() const {
            if constexpr( D == 2 ) {
                auto [i,j] = shape();
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
                auto [i,j,k] = shape();
                ColVectors<unsigned int,2> E(2,
                        (i-1)*(j)*(k)
                        +(i)*(j-1)*(k)
                        +(i)*(j)*(k-1)
                        );

                int counter = 0;
                utils::multi_loop(std::array<size_t,3>{{i-1,j,k}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        unsigned int o = this->index(ii,jj,kk);
                        unsigned int x = this->index(ii+1,jj,kk);
                        E.col(counter++) = Vector<unsigned int,2>(o,x);
                        });
                utils::multi_loop(std::array<size_t,3>{{i,j-1,k}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        unsigned int o = this->index(ii,jj,kk);
                        unsigned int y = this->index(ii,jj+1,kk);
                        E.col(counter++) = Vector<unsigned int,2>(o,y);
                        });
                utils::multi_loop(std::array<size_t,3>{{i,j,k-1}},[&](auto&& ijk) {
                        auto [ii,jj,kk] = ijk;
                        unsigned int o = this->index(ii,jj,kk);
                        unsigned int z = this->index(ii,jj,kk+1);
                        E.col(counter++) = Vector<unsigned int,2>(o,z);
                        });
                return E;
            }
        }
    template <typename T, typename Indexer>
        auto  GridTriangulation<T,Indexer>::vertices() const -> ColVectors<T,D> {
            ColVectors<T,D> V(D,this->size());
            using IndexScalar = typename Indexer::index_type::value_type;
            using IVec = Vector<IndexScalar,D>;
            auto denom = (Eigen::Map<const IVec>(shape().data()) - IVec::Ones()).eval();
            utils::multi_loop(shape(),[&](auto&& idx) {

                    V.col(this->index(idx)) = Eigen::Map<const IVec>(idx.data()).template cast<T>().cwiseQuotient(denom.template cast<T>());
                    });
            return V;
        }
}}}
#pragma GCC diagnostic pop
