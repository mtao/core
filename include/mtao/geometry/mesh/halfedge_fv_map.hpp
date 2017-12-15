#pragma once
#include "halfedge.hpp"
#include <map>

namespace mtao { namespace geometry { namespace mesh {

    std::map<std::tuple<int,int>,int> fv_to_halfedge(const HalfEdgeMesh& hem);
    mtao::ColVectors<int,3> fv_to_halfedge_triangles(const mtao::ColVectors<int,3>& F, const HalfEdgeMesh& hem);
}}}
