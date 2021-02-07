#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <mtao/geometry/mesh/shapes/arrow.hpp>
#include <mtao/eigen/stack.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>
#include <mtao/geometry/mesh/write_ply.hpp>
#include <tbb/parallel_for.h>


int main(int argc, char *argv[]) {
    const std::string input_filename = argv[1];
    const std::string output_prefix = argv[2];
    spdlog::info("Showing particles with velocities");
    auto [V, N] = mtao::geometry::point_cloud::points_and_velocity_from_partio(input_filename);
    spdlog::info("Done reading ");

    int size = V.cols();
    std::vector<mtao::ColVecs3d> NV(size);
    std::vector<mtao::ColVecs3i> NF(size);
    using namespace mtao::geometry::mesh::shapes;


    double scale = std::stof(argv[3]);

    tbb::parallel_for(int(0), size, [&](int i) {
        spdlog::info("Doing {}", i);
        auto &nV = NV[i];
        auto &nF = NF[i];
        auto v = V.col(i);
        auto n = N.col(i);
        mtao::ColVecs3d P = mtao::eigen::hstack(v, v + scale * n);
        std::tie(nV, nF) = arrow(P, .1 * scale, 3, 1.5);
    });
    int globalOff = 0;
    for (int i = 0; i < size; ++i) {
        auto &nV = NV[i];
        auto &nF = NF[i];
        nF.array() += globalOff;
        globalOff += nV.cols();
    }

    auto FV = mtao::eigen::hstack_iter(NV.begin(), NV.end());
    auto FF = mtao::eigen::hstack_iter(NF.begin(), NF.end());
    mtao::geometry::mesh::write_plyD(FV, FF, output_prefix + ".ply");
    mtao::geometry::mesh::write_objD(FV, FF, output_prefix + ".obj");
    return 0;
}
