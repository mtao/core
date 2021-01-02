#pragma once
#include <map>

#include "halfedge.hpp"

namespace mtao::geometry::mesh {

// Maps each facet into its associated halfedge
std::map<std::tuple<int, int>, int> fv_to_halfedge(const HalfEdgeMesh &hem);
ColVectors<int, 3> fv_to_halfedge_triangles(const ColVectors<int, 3> &F,
                                            const HalfEdgeMesh &hem);
}// namespace mtao::geometry::mesh
