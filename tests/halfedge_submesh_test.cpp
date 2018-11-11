#include <iostream>
#include "mtao/geometry/mesh/sphere.hpp"
#include "mtao/geometry/mesh/halfedge.hpp"
#include "mtao/geometry/mesh/halfedge_fv_map.hpp"
#include "mtao/geometry/mesh/read_obj.hpp"


#include "mtao/logging/logger.hpp"
using namespace mtao::logging;


int main(int argc, char * argv[]) {

    mtao::ColVectors<float,3> V;
    mtao::ColVectors<int,3> F;

    using namespace mtao::geometry::mesh;
    if(argc == 1) {
        std::tie(V,F) = sphere<float>(1);
    } else {
        std::tie(V,F) = read_objF(argv[1]);
    }


    HalfEdgeMesh hem = HalfEdgeMesh::from_cells(F);


    std::cout << hem.edges() << std::endl;
    for(int i = 0; i < hem.size(); ++i) {
        std::cout << "edge: " << i << std::endl;
        std::cout << hem.edges().col(i) << std::endl;
        std::cout << "===" << std::endl;
    if(hem.is_boundary(i)) {
    boundary_iterator(hem.edge(i))( [&](auto&& e) {
            std::cout << "[" << e.index() << " " << e.vertex() << "] " << std::endl;

            });
    std::cout << std::endl;
    } else {
        std::cout << "Boundary iterator on non-boundary edge" << std::endl;
    }
    }
    return 0;

}
