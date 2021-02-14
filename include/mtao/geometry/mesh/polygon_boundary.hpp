#pragma once
#include <numeric>
#include <set>
#include <vector>


namespace mtao::geometry::mesh {

// Note that if you want triangulation functionality you should include polygon_boundary_triangulate
struct PolygonBoundaryIndices : std::vector<int> {
    std::set<std::vector<int>> holes;


    using std::vector<int>::vector;
    using Base = std::vector<int>;


    PolygonBoundaryIndices(const PolygonBoundaryIndices &) = default;
    PolygonBoundaryIndices(PolygonBoundaryIndices &&) = default;
    PolygonBoundaryIndices &operator=(const PolygonBoundaryIndices &) = default;
    PolygonBoundaryIndices &operator=(PolygonBoundaryIndices &&) = default;
    PolygonBoundaryIndices(std::vector<int> b = {},
                           std::set<std::vector<int>> h = {})
      : std::vector<int>{ std::move(b) }, holes{ std::move(h) } {}


    std::vector<int> &outer_loop() { return *this; }
    const std::vector<int> &outer_loop() const { return *this; }
    size_t outer_loop_size() const { return Base::size(); }

    bool is_simple() const { return holes.empty(); }

    size_t hole_size() const { return holes.size(); }

    std::set<int> vertices() const {
        std::set<int> v{ Base::begin(), Base::end() };
        for (auto &&h : holes) {
            v.insert(h.begin(), h.end());
        }
        return v;
    }
};


}// namespace mtao::geometry::mesh
