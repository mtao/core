#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <mtao/eigen/stack.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>


int main(int argc, char *argv[]) {
    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];
    if (!mtao::geometry::point_cloud::partio_has_velocity(input_filename)) {
        spdlog::info("Showing just particles");
        auto P = mtao::geometry::point_cloud::points_from_partio(input_filename);
        mtao::geometry::mesh::write_objD(P, mtao::ColVecs2i{}, output_filename);

    } else {
        spdlog::info("Showing particles with velocities");
        auto [P, V] = mtao::geometry::point_cloud::points_and_velocity_from_partio(input_filename);


        int stride = P.cols();

        mtao::ColVecs2i E(2, stride);
        E.row(0) = mtao::VecXi::LinSpaced(stride, 0, stride - 1).transpose();
        E.row(1) = stride + E.row(0).array();


        mtao::geometry::mesh::write_objD(mtao::eigen::hstack(P, P + V), E, output_filename);
    }
}
