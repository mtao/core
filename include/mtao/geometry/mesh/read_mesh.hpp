#pragma once
#include "mtao/types.hpp"
#include "mtao/geometry/mesh/triangle_mesh.hpp"

namespace mtao::geometry::mesh {
    TriangleMesh read_mesh(const std::string& str);
}
