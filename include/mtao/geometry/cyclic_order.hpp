#pragma once
#include <array>
#include <numeric>
#include <vector>

#include "mtao/types.hpp"
#include "mtao/iterator/enumerate.hpp"

namespace mtao::geometry {
constexpr static std::array<int, 4> __quadrants{ { 4, 1, 3, 2 } };
template<typename T>
std::vector<int> cyclic_order(const mtao::ColVectors<T, 2> &V) {
    size_t size = V.cols();
    // sort indices by quadrant and cross product
    std::vector<char> quadrants(size);
    for (size_t i = 0; i < size; ++i) {
        auto b = V.col(i);
        // ++ +- -+ -- => 1 4 2 3
        quadrants[i] =
          __quadrants[2 * std::signbit(b.y()) + std::signbit(b.x())];
    }
    // sort by quadrant and then by cross product volume
    auto comp = [&](int ai, int bi) -> bool {
        const char qa = quadrants[ai];
        const char qb = quadrants[bi];
        if (qa == qb) {
            auto a = V.col(ai);
            auto b = V.col(bi);
            return b.x() * a.y() < a.x() * b.y();
        } else {
            return qa < qb;
        }
    };
    // we need to sort D and quadrants simultaneously, easier to just sort
    // indices into both.
    std::vector<int> ordered_indices(size);
    // spit the initial indices of indices configuration
    std::iota(ordered_indices.begin(), ordered_indices.end(), 0);
    // sort the indices of indices
    std::sort(ordered_indices.begin(), ordered_indices.end(), comp);
    return ordered_indices;
}

template<typename T>
std::vector<int> cyclic_order(const mtao::ColVectors<T, 2> &V, int base, const std::vector<int> &indices) {
    const auto o = V.col(base);
    mtao::ColVecs2d D(2, indices.size());
    for (auto [i, j] : mtao::iterator::enumerate(indices)) {
        D.col(i) = V.col(j) - o;
    }
    std::vector<int> ordered_indices = cyclic_order(D);

    // dereference the indices of indices
    std::transform(ordered_indices.begin(), ordered_indices.end(), ordered_indices.begin(), [&](int idx) -> int { return indices[idx]; });
    return ordered_indices;
}
}// namespace mtao::geometry
