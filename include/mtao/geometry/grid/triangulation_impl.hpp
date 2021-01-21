#include "mtao/geometry/grid/grid_utils.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"


namespace mtao::geometry::grid {

template<typename GridType>
ColVectors<int, 3> GridTriangulator<GridType>::faces() const {

    if constexpr (D == 2) {
        auto [i, j] = g.shape();
        ColVectors<int, 3> F(3, 2 * (i - 1) * (j - 1));
        int counter = 0;
        utils::multi_loop_parallel(std::array<size_t, 2>{ { i - 1, j - 1 } }, [&](auto &&ij) {
            //auto [ii, jj] = ij;
            //std::array<size_t, 4> x{ { g.index(ii, jj), g.index(ii + 1, jj), g.index(ii, jj + 1), g.index(ii + 1, jj + 1) } };
            //F.col(counter++) = Vector<int, 3>(x[0], x[1], x[2]);
            //F.col(counter++) = Vector<int, 3>(x[1], x[2], x[3]);
            auto x = face_loop(ij);
            F.col(counter++) = Vector<int, 3>(x[0], x[1], x[2]);
            F.col(counter++) = Vector<int, 3>(x[0], x[2], x[3]);
        });
        return F;
    } else if constexpr (D == 3) {
        auto [i, j, k] = g.shape();
        ColVectors<int, 3> F(3,
                             2 * ((i) * (j - 1) * (k - 1) + (i - 1) * (j) * (k - 1) + (i - 1) * (j - 1) * (k)));
        int counter = 0;
        utils::multi_loop_parallel(std::array<size_t, 3>{ { i, j - 1, k - 1 } }, [&](auto &&ijk) {
            auto x = face_loop(ijk, 0);
            F.col(counter++) = Vector<int, 3>(x[0], x[1], x[2]);
            F.col(counter++) = Vector<int, 3>(x[0], x[2], x[3]);
        });
        utils::multi_loop_parallel(std::array<size_t, 3>{ { i - 1, j, k - 1 } }, [&](auto &&ijk) {
            auto x = face_loop(ijk, 1);
            F.col(counter++) = Vector<int, 3>(x[0], x[1], x[2]);
            F.col(counter++) = Vector<int, 3>(x[0], x[2], x[3]);
        });
        utils::multi_loop_parallel(std::array<size_t, 3>{ { i - 1, j - 1, k } }, [&](auto &&ijk) {
            auto x = face_loop(ijk, 2);
            F.col(counter++) = Vector<int, 3>(x[0], x[1], x[2]);
            F.col(counter++) = Vector<int, 3>(x[0], x[2], x[3]);
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

template<typename GridType>
auto GridTriangulator<GridType>::face_loop(const std::array<size_t, D> &coord, size_t dim) const -> std::array<size_t, 4> {
    if constexpr (D == 2) {

        auto [ii, jj] = coord;
        return std::array<size_t, 4>{ { g.index(ii, jj), g.index(ii + 1, jj), g.index(ii + 1, jj + 1), g.index(ii, jj + 1) } };
    } else if constexpr (D == 3) {
        auto [ii, jj, kk] = coord;
        switch (dim) {
        case 0:
            return std::array<size_t, 4>{ { g.index(ii, jj, kk), g.index(ii, jj + 1, kk), g.index(ii, jj + 1, kk + 1), g.index(ii, jj, kk + 1) } };
        case 1:
            return std::array<size_t, 4>{ { g.index(ii, jj, kk), g.index(ii, jj, kk + 1), g.index(ii + 1, jj, kk + 1), g.index(ii + 1, jj, kk) } };
        case 2:
            return std::array<size_t, 4>{ { g.index(ii, jj, kk), g.index(ii + 1, jj, kk), g.index(ii + 1, jj + 1, kk), g.index(ii, jj + 1, kk) } };
        default:
            return {};
        }
    }
}
template<typename GridType>
auto GridTriangulator<GridType>::face_loop(size_t index) const -> std::array<size_t, 4> {
    auto [coord, type] = g.template form_unindex<2>(index);
    std::array<size_t,D> c2;
    std::copy(coord.begin(),coord.end(),c2);
    return face_loop(c2, type);
}

}// namespace mtao::geometry::grid
#pragma GCC diagnostic pop
