#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <mtao/geometry/mesh/shapes/sphere.hpp>
#include <mtao/geometry/mesh/write_ply.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>
#include <mtao/eigen/stack.hpp>
#include <iostream>
#include <tbb/parallel_for.h>

#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
    auto [P, V] = mtao::geometry::point_cloud::points_and_velocity_from_partio(argv[1]);
    spdlog::info("Opened a partio file with {} particles", P.cols());

    bool use_color = V.size() > 0;
    int size = V.cols();
    std::vector<mtao::ColVecs3d> NV(size);
    std::vector<mtao::ColVecs3d> NC(size);
    std::vector<mtao::ColVecs3i> NF(size);
    using namespace mtao::geometry::mesh::shapes;


    double scale = std::stof(argv[3]);
    int subdiv = 2;
    if (argc > 4) {
        subdiv = std::atoi(argv[4]);
    }
    auto SVF = sphere<double>(subdiv);

    int globalOff = 0;
    tbb::parallel_for(int(0), size, [&](int i) {
        auto &nV = NV[i];
        auto &nF = NF[i];
        auto v = P.col(i);

        std::tie(nV, nF) = SVF;
        nV *= scale;
        nV.colwise() += v;
        if (use_color) {
            auto &nC = NC[i];
            auto v = V.col(i);
            nC.resizeLike(nV);
            nC.colwise() = v;
        }

        nF.array() += globalOff;
        globalOff += nV.cols();
    });

    auto FV = mtao::eigen::hstack_iter(NV.begin(), NV.end());
    auto FF = mtao::eigen::hstack_iter(NF.begin(), NF.end());
    if (use_color) {
        auto FC = mtao::eigen::hstack_iter(NC.begin(), NC.end());
        mtao::geometry::mesh::write_plyD(FV, FC, FF, std::string(argv[2]) + ".ply");
    } else {
        mtao::geometry::mesh::write_plyD(FV, FF, std::string(argv[2]) + ".ply");
    }
    return 0;
}
