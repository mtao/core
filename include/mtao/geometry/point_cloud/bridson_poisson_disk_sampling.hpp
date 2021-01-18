#pragma once
#include <set>
#include <algorithm>

#include <algorithm>
#include "mtao/geometry/grid/grid.h"
#include "mtao/geometry/grid/grid_data.hpp"
#include <random>
#include <Eigen/Geometry>


namespace mtao::geometry::point_cloud {
template<typename T, int D>
mtao::ColVectors<T, D> bridson_poisson_disk_sampling(const Eigen::AlignedBox<T, D> &bounding_box, T radius, int insert_attempts = 10) {

    std::random_device rd;
    std::mt19937 gen(rd());


    double cell_side_length = radius / std::sqrt(D);
    std::array<int, D> shape;
    auto size = bounding_box.sizes();
    for (int j = 0; j < D; ++j) {
        shape[j] = std::max<int>(1, std::floor(size(j) / cell_side_length));
    }
    const T radius2 = radius * radius;
    auto sample_sphere = [&]() -> mtao::Vector<T, D> {
        mtao::Vector<T, D> R = mtao::Vector<T, D>::Random();
        while (R.squaredNorm() < 1e-5) {
            R = mtao::Vector<T, D>::Random();
        }
        double v = std::sqrt(Eigen::internal::random<T>(radius2, 4 * radius2));
        return v * R.normalized();
    };
    grid::GridDataD<int, D> indices(shape);
    // increase by 1 to get from cell counts to shape
    for (auto &&s : shape) {
        s++;
    }
    auto g = grid::GridD<T, D>::from_bbox(bounding_box, shape, /*cubes = */ true);
    indices.set_constant(-1);

    mtao::ColVectors<T, D> R(D, indices.size());

    R.col(0) = bounding_box.sample();
    auto [c, q] = g.coord(R.col(0));
    indices(c) = 0;


    std::set<int> active_set;
    active_set.emplace(0);
    auto get_random_it = [&]() {
        std::uniform_int_distribution<> distrib(0, active_set.size() - 1);
        auto it = active_set.begin();
        std::advance(it, int(distrib(gen)));
        return it;
    };
    int index = 1;
    for (; !active_set.empty() && index < R.cols();) {
        auto base_it = get_random_it();
        int base_index = *base_it;
        auto p = R.col(base_index);


        auto n = R.col(index);
        //mtao::ColVecs
        int k = 0;

        for (; k < insert_attempts; ++k) {
            n = p + sample_sphere();
            if (!bounding_box.contains(n)) {
                continue;
            }
            auto [c, q] = g.coord(n);

            auto l = c;
            auto h = c;
            for (auto &&v : l) { v--; }
            for (auto &&v : h) { v++; }


            bool has_bad = false;
            mtao::geometry::grid::utils::multi_loop(l, h, [&](auto &&c) {
                if (has_bad) {
                    return;
                }
                if (indices.valid_index(c)) {
                    int &idx = indices(c);
                    if (idx != -1) {
                        if ((R.col(idx) - n).squaredNorm() > radius2) {
                            has_bad = true;
                        }
                    }
                }
            });
            if (!has_bad) {
                int &idx = indices(c);
                if (idx == -1) {
                    idx = index;
                    active_set.emplace(index);
                    index++;
                    break;
                }
            }
        }
        if (k == insert_attempts) {
            active_set.erase(base_it);
        }
    }
    if (index < R.cols()) {
        R.conservativeResize(D, index);
    }
    return R;
}

}// namespace mtao::geometry::point_cloud
