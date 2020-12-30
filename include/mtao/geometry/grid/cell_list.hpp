#pragma once

#include "grid.h"
#include "grid_data.hpp"
#include "mtao/iterator/zip.hpp"

namespace mtao::geometry::grid {
template<typename Scalar, int D>
struct CellList : public GridD<Scalar, D> {
    using GridD<Scalar, D>::shape;
    using GridD<Scalar, D>::coord;
    using GridD<Scalar, D>::index;
    using GridD<Scalar, D>::valid;
    using GridD<Scalar, D>::valid_index;
    using GridD<Scalar, D>::loop;
    using GridD<Scalar, D>::dx;
    using coord_type = typename GridD<Scalar, D>::coord_type;
    CellList(const GridD<Scalar, D> &g) : GridD<Scalar, D>(g) {
        offsets = GridDataD<int, D>::Constant(-1, shape());
    }

    template<typename Derived>
    void construct(const Eigen::MatrixBase<Derived> &P);
    // takes afunctor that just wants a bunch of ints passed in
    template<typename Derived, typename Func>
    void neighbor_walker(Func &&f, const Eigen::MatrixBase<Derived> &p, double radius) const;
    template<typename Func>
    void neighbor_walker(Func &&f, const coord_type &c, int radius = 1) const;
    template<typename Func>
    void neighbor_walker(Func &&f, const coord_type &c, const coord_type &radius) const;
    GridDataD<int, D> offsets;
    std::vector<int> particle_table;
    int out_of_bound_offset = -1;
};
template<typename Scalar, int D>
template<typename Derived>
void CellList<Scalar, D>::construct(const Eigen::MatrixBase<Derived> &P) {
    // dumping all out of bound values to the last bucket
    particle_table.resize(P.cols() + 1);
    auto it = particle_table.begin();
    for (int j = 0; j < P.cols(); ++j) {
        auto [c, q] = coord(P.col(j));
        bool valid_ = valid_index(c);
        int index = valid_ ? j : P.cols();

        int &off = valid_ ? offsets(c) : out_of_bound_offset;
        *it++ = off;
        off = index;
    }
}
template<typename Scalar, int D>
template<typename Derived, typename Func>
void CellList<Scalar, D>::neighbor_walker(Func &&f,
                                          const Eigen::MatrixBase<Derived> &p,
                                          double radius) const {
    auto [c, q] = coord(p);
    coord_type rad;
    auto dx = this->dx();
    for (int j = 0; j < D; ++j) {
        rad[j] = std::ceil(radius / dx[j]);
    }
    neighbor_walker(f, c, rad);
}
template<typename Scalar, int D>
template<typename Func>
void CellList<Scalar, D>::neighbor_walker(Func &&f, const coord_type &c, int radius) const {
    coord_type r;
    std::fill(r.begin(), r.end(), radius);
    return neighbor_walker(f, c, r);
}
template<typename Scalar, int D>
template<typename Func>
void CellList<Scalar, D>::neighbor_walker(Func &&f, const coord_type &c, const coord_type &radius) const {
    std::array<int, D> start = c;
    for (auto &&[v, r] : mtao::iterator::zip(start, radius)) {
        v -= r;
    }
    std::array<int, D> end = c;
    for (auto &&[v, r] : mtao::iterator::zip(end, radius)) {
        v += r + 1;
    }

    bool needs_invalid_checks = false;
    auto run = [&](int index) {
        for (; index >= 0; index = particle_table.at(index)) {
            f(index);
        }
    };
    utils::multi_loop(start, end, [&](auto &&c) {
        if (valid_index(c)) {
            run(offsets(c));
        } else {
            needs_invalid_checks = true;
        }
    });
    if (needs_invalid_checks) {
        run(out_of_bound_offset);
    }
}
}// namespace mtao::geometry::grid
