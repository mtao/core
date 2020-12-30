#include <iostream>

#include "mtao/geometry/mesh/sphere.hpp"
#include "mtao/geometry/mesh/mean_curvature_normals.hpp"


int main() {

    {
        mtao::ColVectors<float, 3> V;
        mtao::ColVectors<int, 3> F;

        std::tie(V, F) = mtao::geometry::mesh::sphere<float>(4);
        std::cout << V.col(0).transpose() << std::endl;


        auto N = mtao::geometry::mesh::mean_curvature_normals(V, F);
        for (int i = 0; i < N.cols(); ++i) {
            std::cout << N.col(i).dot(V.col(i)) << " ";
        }
        std::cout << std::endl;

        {
            auto N = mtao::geometry::mesh::mean_curvature_normals(V, F, false);
            std::cout << N.colwise().sum() << std::endl;
        }
        {
            auto N = mtao::geometry::mesh::mean_curvature_normals(.5 * V, F, false);
            std::cout << N.colwise().sum() << std::endl;
        }
        {
            auto N = mtao::geometry::mesh::mean_curvature_normals(.1 * V, F, false);
            std::cout << N.colwise().sum() << std::endl;
        }
    }

    return 0;
}
