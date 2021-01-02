#pragma once

#include <mtao/geometry/mesh/shapes/arrow.hpp>
#include <mtao/iterator/enumerate.hpp>


namespace mtao::geometry::mesh::shapes {
template<typename T>
std::tuple<mtao::ColVectors<T, 3>, mtao::ColVecs3i> vector_field(const mtao::ColVectors<T, 3> &P, const mtao::ColVectors<T, 3> &V, double cone_radius, int N = 5, double arrow_radius_scale = 1.5, double tip_scale = 1.) {
    assert(P.cols() == V.cols());
    std::vector<mtao::ColVectors<T, 3>> Vs(P.cols());
    std::vector<mtao::ColVectors<int, 3>> Fs(P.cols());
    int offset = 0;
    for (auto &&[i, v, f] : mtao::iterator::enumerate(Vs, Fs)) {
        mtao::ColVectors<T, 3> A(3, 2);
        A.col(0) = P.col(i);
        A.col(1) = P.col(i) + V.col(i);
        std::tie(v, f) = arrow(A, cone_radius, N, arrow_radius_scale, tip_scale);

        f.array() += offset;
        offset += v.cols();
    }
    return {
        mtao::eigen::hstack_iter(Vs.begin(), Vs.end()),
        mtao::eigen::hstack_iter(Fs.begin(), Fs.end())
    };
}
}// namespace mtao::geometry::mesh::shapes
