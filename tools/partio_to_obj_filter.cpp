#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <mtao/iterator/enumerate.hpp>
#include <mtao/eigen/stack.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>
#include <vector>


int main(int argc, char *argv[]) {
    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];
    int axis = std::atoi(argv[3]);
    
    //if (!mtao::geometry::point_cloud::partio_has_velocity(input_filename)) {
        spdlog::info("Showing just particles");
        auto P = mtao::geometry::point_cloud::points_from_partio(input_filename);
        std::vector<int> active;
        active.reserve(P.cols()/2);
        for(int j = 0; j < P.cols(); ++j) {
            auto p = P.col(j);
            if(p(axis) < 0) {
                active.emplace_back(j);
            }
        }
        mtao::ColVecs3d P2(3,active.size());
        for(auto&& [i,j]: mtao::iterator::enumerate(active)) {
            P2.col(i) = P.col(j);
        }

        mtao::geometry::mesh::write_objD(P2, mtao::ColVecs2i{}, output_filename);

        /*
    } else {
        spdlog::info("Showing particles with velocities");
        auto [P, V] = mtao::geometry::point_cloud::points_and_velocity_from_partio(input_filename);


        int stride = P.cols();

        mtao::ColVecs2i E(2, stride);
        E.row(0) = mtao::VecXi::LinSpaced(stride, 0, stride - 1).transpose();
        E.row(1) = stride + E.row(0).array();


        mtao::geometry::mesh::write_objD(mtao::eigen::hstack(P, P + V), E, output_filename);
    }
    */
}
