
#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <mtao/eigen/stack.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>


int main(int argc, char *argv[]) {
    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];
    mtao::ColVecs3d P;
    mtao::ColVecs3d V;
    if (!mtao::geometry::point_cloud::partio_has_velocity(input_filename)) {
        spdlog::info("Showing just particles");
        P = mtao::geometry::point_cloud::points_from_partio(input_filename);

    } else {
        spdlog::info("Showing particles with velocities");
        std::tie(P, V) = mtao::geometry::point_cloud::points_and_velocity_from_partio(input_filename);
    }
}
