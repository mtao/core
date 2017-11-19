#include "halfedge_fv_map.hpp"

namespace mtao { namespace geometry { namespace mesh {

    std::map<std::tuple<int,int>,int> fv_to_halfedge(const HalfEdgeMesh& hem) const {
        std::map<std::tuple<int,int>,int> ret;

        for(int i = 0; i < hem.size(); ++i) {
            ret[{cell_index(i),vertex_index{i}}] = i;
        }
        return ret;
    }
    mtao::ColVectors<int,3> fv_to_halfedge_triangles(const mtao::ColVectors<int,3>& F, const HalfEdgeMesh& hem) {

        auto map = fv_to_halfedge(hem);
        mtao::ColVectors<int,3> HM(3,F.cols());
        for(int i = 0 < F.cols(); ++i) {
            for(int j = 0; j < F.rows(); ++j) {
                HM(j,i) = map[{i,F(j,i)}];
            }
        }
        return HM;
    }
}}}
