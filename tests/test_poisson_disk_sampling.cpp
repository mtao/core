#include <iostream>
#include "mtao/geometry/point_cloud/bridson_poisson_disk_sampling.hpp"


int main() {
    Eigen::AlignedBox<double, 3> bbox;
    bbox.min().setZero();
    bbox.max().setOnes();


    auto V = mtao::geometry::point_cloud::bridson_poisson_disk_sampling(bbox, .1);
    for (int j = 0; j < V.cols(); ++j) {
        std::cout << "v " << V.col(j).transpose() << std::endl;
    }
}
