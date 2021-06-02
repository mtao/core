#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <mtao/eigen/stack.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>


int main(int argc, char *argv[]) {
    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];

    double scale = .1;
    if (argc > 3) {
        scale = std::atof(argv[3]);
    }
    int subdiv = 2;
    if (argc > 4) {
        subdiv = std::atoi(argv[4]);
    }
    spdlog::info("Showing particles with velocities");
    auto [P, V] = mtao::geometry::point_cloud::points_and_velocity_from_partio(input_filename);

    int size = V.cols();

    int stride = P.cols();

    auto SVF = sphere<double>(subdiv);

    int globalOff = 0;
    for (int i = 0; i < size; ++i) {
        auto v = V.col(i);

        auto pts = (scale * V).colwise() + v;
        for (int j = 0; j < pts.cols(); ++j) {
            fmt::print("v {}\n", fmt::join(pts.col(j), " "));
        }
        fmt::print("vn {}\n", fmt::join(V.col(i), " "));


        auto vend = fmt::format("/{}", i + 1);
        for(int j = 0; j < 
        nF.array() += globalOff;
        globalOff += stride;
    }
}
