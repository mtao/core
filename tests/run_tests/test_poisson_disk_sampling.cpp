#include <iostream>
#include "mtao/geometry/point_cloud/bridson_poisson_disk_sampling.hpp"


int main() {
    Eigen::AlignedBox<double, 2> bbox;
    bbox.min().setZero();
    bbox.max().setOnes();


    double radius = .2;
    auto V = mtao::geometry::point_cloud::bridson_poisson_disk_sampling(bbox, radius);
    std::cout << V << std::endl;
    for (int j = 0; j < V.cols(); ++j) {
        for (int k = j + 1; k < V.cols(); ++k) {
            if ((V.col(j) - V.col(k)).norm() < radius) {
                std::cout << "Failure: " << j << ", " << k << std::endl;
            }
        }
    }
}
