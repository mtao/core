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


    /*
    F.resize(3,2);
    F(0,0) = 0;
    F(1,0) = 1;
    F(2,0) = 2;
    F(0,1) = 2;
    F(1,1) = 1;
    F(2,1) = 3;
    */
    HalfEdgeMesh hem = HalfEdgeMesh::from_cells(F);

    debug() << "boundary size: " << hem.boundary_size();
    auto C = hem.cells();
    for(size_t i = 0; i < C.size(); ++i) {
        std::cout << F.col(i).transpose() << std::endl;
        std::cout << "=============" << std::endl;
                cell_iterator(&hem,C[i])([&](auto&& e) {
            std::cout << e.vertex() << " ";
                        });
        std::cout << std::endl;
        std::cout << std::endl << std::endl;
    }
    auto D = hem.vertices();

    for(size_t i = 0; i < C.size(); ++i) {
        std::cout << "dual cell: " << i << ") ";
                vertex_iterator(&hem,C[i])([&](auto&& e) {
                        std::cout << e.cell() << " ";
                        });
        std::cout << std::endl;
    }


    std::cout << "dual cell count: " << D.size() << " / " << V.cols() << std::endl;


    std::cout << "hem corner vertex: ";
    vertex_iterator(&hem, hem.vertex_edge(21).index())( [&](auto&& e) {
            std::cout << "[" << e.dual_index() << " " << e.vertex() << "] ";
            });
    std::cout << std::endl;

    auto fv = fv_to_halfedge(hem);
    return 0;

    for(int i = 0; i < hem.size(); ++i) {
    if(hem.is_boundary(i)) {
    boundary_iterator(hem.edge(i))( [&](auto&& e) {
            std::cout << "[" << e.dual_index() << " " << e.vertex() << "] ";
            });
    std::cout << std::endl;
    } else {
        std::cout << "Boundary iterator on non-boundary edge" << std::endl;
    }
    }
    return 0;

}
