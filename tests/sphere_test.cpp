#include <iostream>
#include "geometry/mesh/sphere.hpp"



int main() {

    mtao::ColVectors<float,3> V;
    mtao::ColVectors<int,3> F;

    std::tie(V,F) = mtao::geometry::mesh::sphere<float>(3);
    std::cout << V << std::endl;
    std::cout << F << std::endl;

    return 0;

}
