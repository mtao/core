#include <mtao/geometry/mesh/boundary_matrix.h>
#include <mtao/types.hpp>
#include <iostream>


int main(int argc, char *argv[]) {
    mtao::ColVecs2i E(2, 5);
    E << 0, 1, 2, 3, 4,
      1, 0, 3, 4, 1;
    std::cout << E << std::endl;
    std::cout << mtao::geometry::mesh::graph_boundary_matrix<float>(E) << std::endl;

    return 0;
}
