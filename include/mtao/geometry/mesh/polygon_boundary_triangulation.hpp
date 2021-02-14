
#pragma once
#include <numeric>
#include <vector>
#include "mtao/types.hpp"
#include "mtao/eigen/shape_checks.hpp"
#include "mtao/geometry/mesh/polygon_boundary.hpp"
#include "mtao/geometry/mesh/earclipping.hpp"


namespace mtao::geometry::mesh {

mtao::ColVecs3i triangulate_fan(const PolygonBoundaryIndices &PBI);

template<eigen::concepts::ColVecs2Compatible ColVecs>
mtao::ColVecs3i triangulate_earclipping(const PolygonBoundaryIndices &PBI, const ColVecs &V) {

    if (!PBI.is_simple()) {
        throw std::invalid_argument("Cannot compute a triangle fan with holes");
    }
    return earclipping(V, PBI.outer_loop());
}

// Try to always put YY at the end of opts for this function. that keeps steiner points away (steiner points wil cause a throw)
mtao::ColVecs3i triangulate_triangle(const PolygonBoundaryIndices &PBI, const mtao::ColVecs2d &V, std::string_view opts = "pcePzQYY");
std::tuple<mtao::ColVecs2d, mtao::ColVecs3i> triangulate_triangle_with_steiner(const PolygonBoundaryIndices &PBI, const mtao::ColVecs2d &V, std::string_view opts = "zPa.01qepcDQ");
}// namespace mtao::geometry::mesh
