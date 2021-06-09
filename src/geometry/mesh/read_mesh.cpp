#include "mtao/geometry/mesh/read_mesh.hpp"
#include "mtao/geometry/mesh/read_obj.hpp"
#include "mtao/geometry/mesh/read_ply.hpp"

namespace mtao::geometry::mesh {
    TriangleMesh read_mesh(const std::string& str);
}
