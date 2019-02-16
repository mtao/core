#include <iostream>

#include "mtao/geometry/mesh/sphere.hpp"
#include "mtao/geometry/mesh/vertex_normals.hpp"


int main() {

    {
        mtao::ColVectors<float,3> V;
        mtao::ColVectors<int,3> F;

        std::tie(V,F) = mtao::geometry::mesh::sphere<float>(4);
        std::cout << V.col(0).transpose() << std::endl;

        
        auto N = mtao::geometry::mesh::vertex_normals(V,F);
        for(int i = 0; i < N.cols(); ++i) {
            std::cout << N.col(i).dot(V.col(i)) << " ";
        }
        std::cout << std::endl;
    }
    {
        mtao::ColVectors<float,2> V;
        mtao::ColVectors<int,2> F;

        std::tie(V,F) = mtao::geometry::mesh::sphere<float,2>(10);

        auto N = mtao::geometry::mesh::vertex_normals(V,F);
        for(int i = 0; i < N.cols(); ++i) {
            std::cout << N.col(i).dot(V.col(i)) << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}

