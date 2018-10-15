#include <iostream>
#include "mtao/geometry/mesh/halfedge.hpp"
#include "mtao/geometry/mesh/halfedge_fv_map.hpp"

#include "mtao/logging/logger.hpp"
using namespace mtao::logging;


int main(int argc, char * argv[]) {

    mtao::ColVectors<float,2> V(2,4);
    mtao::ColVectors<int,2> E(2,V.cols() + V.cols() - 3);

    float dt = 2*M_PI / V.cols();
    for(int i = 0; i < V.cols(); ++i) {
        auto v = V.col(i);
        v(0) = std::cos(i*dt);
        v(1) = std::sin(i*dt);
        auto e = E.col(i);
        e(0) = i;
        e(1) = (i+1)%V.cols();

        

    }
    for(int i = 2; i < V.cols() - 1; ++i) {
        auto e = E.col(V.cols() + i - 2);
        e(0) = 0;
        e(1) = i;
        std::cout << "Cross: " << e.transpose() << std::endl;
    }
    std::cout << E << std::endl;

    using namespace mtao::geometry::mesh;
    HalfEdgeMesh hem(E);

    std::cout << hem.edges() << std::endl << std::endl;



    hem.make_topology(V);
    std::cout << hem.edges() << std::endl << std::endl;

    auto C = hem.cells();
    for(int i = 0; i < C.size(); ++i) {
        cell_iterator(&hem,C[i])([&](auto&& e) {
                std::cout << e.vertex() << " ";
                });
        std::cout << std::endl;
        std::cout << std::endl << std::endl;
    }


    


}

