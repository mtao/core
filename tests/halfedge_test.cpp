#include <iostream>
#include "geometry/mesh/sphere.hpp"
#include "geometry/mesh/halfedge.hpp"



int main() {

    mtao::ColVectors<float,3> V;
    mtao::ColVectors<int,3> F;

    using namespace mtao::geometry::mesh;
    std::tie(V,F) = sphere<float>(3);

    HalfEdgeMesh hem(F);

    auto C = hem.cells();
    for(int i = 0; i < C.size(); ++i) {
        std::cout << F.col(i).transpose() << std::endl;
        std::cout << "=============" << std::endl;
                cell_iterator(&hem,C[i])([&](auto&& e) {
            std::cout << e.vertex() << " ";
                        });
        std::cout << std::endl;
        std::cout << std::endl << std::endl;
    }
    auto D = hem.dual_cells();

    for(int i = 0; i < C.size(); ++i) {
                dual_cell_iterator(&hem,C[i])([&](auto&& e) {
                        std::cout << e.get_dual().vertex() << " ";
                        });
        std::cout << std::endl;
    }

    std::cout << "dual cell count: " << D.size() << " / " << V.cols() << std::endl;

    std::cout << hem.vertex_indices() << std::endl;

    return 0;

}
