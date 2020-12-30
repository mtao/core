#include <mtao/geometry/point_cloud/read_xyz.hpp>
#include <mtao/geometry/mesh/shapes/sphere.hpp>
#include <mtao/geometry/mesh/write_ply.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>
#include <mtao/eigen/stack.hpp>
#include <iostream>


int main(int argc, char *argv[]) {
    auto [V, N] = mtao::geometry::point_cloud::read_xyzD(argv[1]);

    bool use_color = N.size() > 0;
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
    for (int i = 0; i < size; ++i) {
        auto &nV = NV[i];
        auto &nF = NF[i];
        auto v = V.col(i);

        std::tie(nV, nF) = SVF;
        nV *= scale;
        nV.colwise() += v;
        if (use_color) {
            auto &nC = NC[i];
            auto n = N.col(i);
            nC.resizeLike(nV);
            nC.colwise() = n;
        }

        nF.array() += globalOff;
        globalOff += nV.cols();
    }

    auto FV = mtao::eigen::hstack_iter(NV.begin(), NV.end());
    auto FF = mtao::eigen::hstack_iter(NF.begin(), NF.end());
    if (use_color) {
        auto FC = mtao::eigen::hstack_iter(NC.begin(), NC.end());
        mtao::geometry::mesh::write_plyD(FV, FC, FF, std::string(argv[2]) + ".ply");
    } else {
        mtao::geometry::mesh::write_plyD(FV, FF, std::string(argv[2]) + ".ply");
    }
    mtao::geometry::mesh::write_objD(FV, FF, std::string(argv[2]) + ".obj");
    return 0;
}
