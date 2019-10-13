#include <mtao/geometry/point_cloud/read_xyz.hpp>
#include <mtao/geometry/mesh/shapes/arrow.hpp>
#include <mtao/geometry/mesh/write_ply.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>
#include <iostream>


int main(int argc, char * argv[]) {
    auto [V,N] = mtao::geometry::point_cloud::read_xyzD(argv[1]);

    int size = V.cols();
    std::vector<mtao::ColVecs3d> NV(size);
    std::vector<mtao::ColVecs3i> NF(size);
    using namespace mtao::geometry::mesh::shapes;


    double scale = std::stof(argv[3]);

    int globalOff = 0;
    for(int i = 0; i < size; ++i) {
        auto& nV = NV[i];
        auto& nF = NF[i];
        auto v = V.col(i);
        auto n = N.col(i);
        mtao::ColVecs3d P = mtao::eigen::hstack(v,v+scale * n);
        std::tie(nV,nF) = arrow(P,.1 * scale,20,1.5);
        nF.array() += globalOff;
        globalOff += nV.cols();
    }

    auto FV = mtao::eigen::hstack_iter(NV.begin(),NV.end());
    auto FF = mtao::eigen::hstack_iter(NF.begin(),NF.end());
    mtao::geometry::mesh::write_plyD(FV,FF,std::string(argv[2])+".ply");
    mtao::geometry::mesh::write_objD(FV,FF,std::string(argv[2])+".obj");
    return 0;
}
