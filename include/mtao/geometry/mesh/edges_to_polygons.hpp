#pragma once
#include <set>
#include <tuple>
#include <vector>

#include "mtao/algebra/partial_order_to_dag.hpp"
#include "mtao/geometry/mesh/edges_to_plcurves.hpp"
#include "mtao/geometry/winding_number.hpp"

namespace mtao::geometry::mesh {

// converts a collection of curves represented as pairs of indices in E into a
// collection of closed polygons. This function supports general polygons, which
// are demarked as an outer boundary loop + the holes inside of it
struct PolygonBoundaryIndices : std::vector<int> {
    using std::vector<int>::vector;
    PolygonBoundaryIndices() = default;
    PolygonBoundaryIndices(const PolygonBoundaryIndices&) = default;
    PolygonBoundaryIndices(PolygonBoundaryIndices&&) = default;
    PolygonBoundaryIndices(std::vector<int> b,
                           std::set<std::vector<int>> h = {})
        : std::vector<int>{std::move(b)}, holes{std::move(h)} {}
    std::set<std::vector<int>> holes;
};
template <typename VDerived>
std::vector<PolygonBoundaryIndices> edges_to_polygons(
    const Eigen::MatrixBase<VDerived>& V,
    const std::vector<std::tuple<std::vector<int>, bool>>& loops);
template <typename VDerived, typename EDerived>
std::vector<PolygonBoundaryIndices> edges_to_polygons(
    const Eigen::MatrixBase<VDerived>& V,
    const Eigen::MatrixBase<EDerived>& E) {
    eigen::col_check<2>(V);
    eigen::col_check<2>(E);

    auto loops = edges_to_plcurves(E, true);
    return edges_to_polygons(V, loops);
}

namespace internal {

// comparitor that takes in a vertex colvector object and uses that to compare
// boundary loop indices symmetric means that it takes the extra effort to check
// that a is inside b and b is inside a
template <bool Symmetric, typename VDerived>
struct winding_number_loop_comparator {
    const VDerived& V;
    template <typename Container>
    std::partial_ordering operator()(const Container& a,
                                     const Container& b) const;
};
}  // namespace internal

template <typename VDerived>
std::vector<PolygonBoundaryIndices> edges_to_polygons(
    const Eigen::MatrixBase<VDerived>& V,
    const std::vector<std::tuple<std::vector<int>, bool>>& loops) {
    eigen::col_check<2>(V);

    auto loop_compare_op =
        internal::winding_number_loop_comparator<false, VDerived>{V};
    auto op = [&](size_t a, size_t b) -> std::partial_ordering {
        const auto& loopa = std::get<0>(loops.at(a));
        const auto& loopb = std::get<0>(loops.at(b));
        return loop_compare_op(loopa, loopb);
    };
    std::vector<size_t> indices(loops.size());
    std::iota(indices.begin(), indices.end(), 0);

    auto dag = algebra::partial_order_to_dag(indices, op);
    std::vector<PolygonBoundaryIndices> ret;
    ret.reserve(loops.size());

    for (auto&& node : dag.nodes()) {
        PolygonBoundaryIndices pbi(std::get<0>(loops.at(node->value)));
        std::transform(
            node->children.begin(), node->children.end(),
            std::inserter(pbi.holes, pbi.holes.end()),
            [&](auto&& ptr) { return std::get<0>(loops.at(ptr->value)); });
        ret.emplace_back(std::move(pbi));
    }
    return ret;
}

namespace internal {
template <bool Symmetric, typename VDerived>
template <typename Container>
std::partial_ordering
winding_number_loop_comparator<Symmetric, VDerived>::operator()(
    const Container& a, const Container& b) const {
    using IndSet = std::set<typename Container::value_type>;
    IndSet as(a.begin(), a.end()), bs(b.begin(), b.end());
    IndSet diff;
    std::set_intersection(as.begin(), as.end(), bs.begin(), bs.end(),
                          std::inserter(diff, diff.end()));
    IndSet auniq, buniq;

    auto is_inside = [&](IndSet& a, const Container& B) -> bool {
        return std::all_of(a.begin(), a.end(),
                           [&](typename Container::value_type index) -> bool {
                               return geometry::winding_number(V, B,
                                                               V.col(index));
                           });
    };

    // get the unique indices in either set
    std::set_difference(as.begin(), as.end(), diff.begin(), diff.end(),
                        std::inserter(auniq, auniq.end()));
    std::set_difference(bs.begin(), bs.end(), diff.begin(), diff.end(),
                        std::inserter(buniq, buniq.end()));

    if (as.empty() && bs.empty()) {
        return std::partial_ordering::equivalent;
    } else if (as.empty()) {
        // just check whether buniq is inside a or not
        if (is_inside(bs, a)) {
            return std::partial_ordering::greater;
        }
    } else if (bs.empty()) {
        // just check whether auniq is inside b or not
        if (is_inside(as, b)) {
            return std::partial_ordering::less;
        }
    } else {
        // have to check if auniq in b (or buniq in a)
        if (is_inside(bs, a)) {
            return std::partial_ordering::greater;
        } else if (is_inside(as, b)) {
            return std::partial_ordering::less;
        }
    }
    return std::partial_ordering::unordered;
}
}  // namespace internal
}  // namespace mtao::geometry::mesh
