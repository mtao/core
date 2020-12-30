#include "mtao/geometry/grid/grid_utils.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"


namespace mtao {
namespace geometry {
    namespace grid {

        template<typename GridType>
        ColVectors<int, 3> GridTriangulator<GridType>::faces() const {

            if constexpr (D == 2) {
                auto [i, j] = g.shape();
                ColVectors<int, 3> F(3, 2 * (i - 1) * (j - 1));
                int counter = 0;
                utils::multi_loop_parallel(std::array<size_t, 2>{ { i - 1, j - 1 } }, [&](auto &&ij) {
                    auto [ii, jj] = ij;
                    std::array<size_t, 4> x{ { g.index(ii, jj), g.index(ii + 1, jj), g.index(ii, jj + 1), g.index(ii + 1, jj + 1) } };
                    F.col(counter++) = Vector<int, 3>(x[0], x[1], x[2]);
                    F.col(counter++) = Vector<int, 3>(x[1], x[2], x[3]);
                });
                return F;
            } else if constexpr (D == 3) {
                auto [i, j, k] = g.shape();
                ColVectors<int, 3> F(3,
                                     2 * ((i) * (j - 1) * (k - 1) + (i - 1) * (j) * (k - 1) + (i - 1) * (j - 1) * (k)));
                int counter = 0;
                utils::multi_loop_parallel(std::array<size_t, 3>{ { i, j - 1, k - 1 } }, [&](auto &&ijk) {
                    auto [ii, jj, kk] = ijk;
                    std::array<size_t, 4> x{ { g.index(ii, jj, kk), g.index(ii, jj + 1, kk), g.index(ii, jj, kk + 1), g.index(ii, jj + 1, kk + 1) } };
                    F.col(counter++) = Vector<int, 3>(x[0], x[1], x[2]);
                    F.col(counter++) = Vector<int, 3>(x[1], x[2], x[3]);
                });
                utils::multi_loop_parallel(std::array<size_t, 3>{ { i - 1, j, k - 1 } }, [&](auto &&ijk) {
                    auto [ii, jj, kk] = ijk;
                    std::array<size_t, 4> x{ { g.index(ii, jj, kk), g.index(ii + 1, jj, kk), g.index(ii, jj, kk + 1), g.index(ii + 1, jj, kk + 1) } };
                    F.col(counter++) = Vector<int, 3>(x[0], x[1], x[2]);
                    F.col(counter++) = Vector<int, 3>(x[1], x[2], x[3]);
                });
                utils::multi_loop_parallel(std::array<size_t, 3>{ { i - 1, j - 1, k } }, [&](auto &&ijk) {
                    auto [ii, jj, kk] = ijk;
                    std::array<size_t, 4> x{ { g.index(ii, jj, kk), g.index(ii + 1, jj, kk), g.index(ii, jj + 1, kk), g.index(ii + 1, jj + 1, kk) } };
                    F.col(counter++) = Vector<int, 3>(x[0], x[1], x[2]);
                    F.col(counter++) = Vector<int, 3>(x[1], x[2], x[3]);
                });
                return F;
            }
        }
        template<typename GridType>
        ColVectors<int, 2> GridTriangulator<GridType>::edges() const {
            if constexpr (D == 2) {
                auto [i, j] = g.shape();
                ColVectors<int, 2> E(2,
                                     (i - 1) * (j)
                                       + (i) * (j - 1));

                int counter = 0;
                utils::multi_loop_parallel(std::array<size_t, 2>{ { i - 1, j } }, [&](auto &&ij) {
                    auto [ii, jj] = ij;
                    int o = g.index(ii, jj);
                    int x = g.index(ii + 1, jj);
                    E.col(counter++) = Vector<int, 2>(o, x);
                });
                utils::multi_loop_parallel(std::array<size_t, 2>{ { i, j - 1 } }, [&](auto &&ij) {
                    auto [ii, jj] = ij;
                    int o = g.index(ii, jj);
                    int y = g.index(ii, jj + 1);
                    E.col(counter++) = Vector<int, 2>(o, y);
                });
                return E;
            } else if constexpr (D == 3) {
                auto [i, j, k] = g.shape();
                ColVectors<int, 2> E(2,
                                     (i - 1) * (j) * (k)
                                       + (i) * (j - 1) * (k)
                                       + (i) * (j) * (k - 1));

                int counter = 0;
                utils::multi_loop_parallel(std::array<size_t, 3>{ { i - 1, j, k } }, [&](auto &&ijk) {
                    auto [ii, jj, kk] = ijk;
                    int o = g.index(ii, jj, kk);
                    int x = g.index(ii + 1, jj, kk);
                    E.col(counter++) = Vector<int, 2>(o, x);
                });
                utils::multi_loop_parallel(std::array<size_t, 3>{ { i, j - 1, k } }, [&](auto &&ijk) {
                    auto [ii, jj, kk] = ijk;
                    int o = g.index(ii, jj, kk);
                    int y = g.index(ii, jj + 1, kk);
                    E.col(counter++) = Vector<int, 2>(o, y);
                });
                utils::multi_loop_parallel(std::array<size_t, 3>{ { i, j, k - 1 } }, [&](auto &&ijk) {
                    auto [ii, jj, kk] = ijk;
                    int o = g.index(ii, jj, kk);
                    int z = g.index(ii, jj, kk + 1);
                    E.col(counter++) = Vector<int, 2>(o, z);
                });
                return E;
            }
        }
        template<typename GridType>
        auto GridTriangulator<GridType>::vertices() const -> ColVectors<T, D> {
            return g.vertices();
        }
    }// namespace grid
}// namespace geometry
}// namespace mtao
#pragma GCC diagnostic pop
