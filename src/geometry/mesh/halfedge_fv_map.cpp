#include "mtao/geometry/mesh/halfedge_fv_map.hpp"

namespace mtao::geometry::mesh {

std::map<std::tuple<int, int>, int> fv_to_halfedge(const HalfEdgeMesh& hem) {
    std::map<std::tuple<int, int>, int> ret;

    for (int i = 0; i < hem.size(); ++i) {
        ret[{hem.cell_index(i), hem.vertex_index(i)}] = i;
    }
    return ret;
}
ColVectors<int, 3> fv_to_halfedge_triangles(const ColVectors<int, 3>& F,
                                            const HalfEdgeMesh& hem) {
    auto map = fv_to_halfedge(hem);
    ColVectors<int, 3> HM(3, F.cols());
    for (int i = 0; i < F.cols(); ++i) {
        for (int j = 0; j < F.rows(); ++j) {
            HM(j, i) = map[{i, F(j, i)}];
        }
    }
    return HM;
}
}  // namespace mtao::geometry::mesh
