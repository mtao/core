#include <iostream>
#include "mtao/geometry/mesh/sphere.hpp"
#include "mtao/geometry/mesh/halfedge.hpp"
#include "mtao/geometry/mesh/halfedge_fv_map.hpp"
#include "mtao/geometry/mesh/read_obj.hpp"


#include "mtao/logging/logger.hpp"
using namespace mtao::logging;


int main(int argc, char * argv[]) {
    using namespace mtao::geometry::mesh;

    mtao::ColVectors<float,2> V(2,4);
    mtao::ColVectors<int,3> F;

    V.col(0) = mtao::Vec2f(0.0,0.0);
    V.col(1) = mtao::Vec2f(1.0,0.0);
    V.col(2) = mtao::Vec2f(0.0,1.0);
    V.col(3) = mtao::Vec2f(1.0,1.0);

    F.resize(3,2);
    F(0,0) = 0;
    F(1,0) = 1;
    F(2,0) = 2;
    F(0,1) = 2;
    F(1,1) = 1;
    F(2,1) = 3;
    EmbeddedHalfEdgeMesh hem(V,HalfEdgeMesh::from_cells(F));

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
    auto D = hem.V();


    mtao::ColVectors<float,2> P(2,7);

    P.col(0) = mtao::Vec2f(-1.0,0.0);
    P.col(1) = mtao::Vec2f(0.5,0.0);
    P.col(2) = mtao::Vec2f(0.5,0.5);
    P.col(3) = mtao::Vec2f(.2,.3);
    P.col(4) = mtao::Vec2f(.3,.2);
    P.col(5) = mtao::Vec2f(.7,.8);
    P.col(6) = mtao::Vec2f(.7,.8);

    std::cout << hem.get_cells(P).transpose() << std::endl;
    std::cout << hem.get_cell( mtao::Vec2f(-1.0,0.0)) << std::endl;
    std::cout << hem.get_cell( mtao::Vec2f(0.5,0.0) )<< std::endl;
    std::cout << hem.get_cell( mtao::Vec2f(0.5,0.5) )<< std::endl;
    std::cout << hem.get_cell( mtao::Vec2f(.2,.3) )<< std::endl;
    std::cout << hem.get_cell( mtao::Vec2f(.3,.2) )<< std::endl;
    std::cout << hem.get_cell( mtao::Vec2f(.7,.8) )<< std::endl;
    std::cout << hem.get_cell( mtao::Vec2f(.7,.8) )<< std::endl;

    return 0;

}
