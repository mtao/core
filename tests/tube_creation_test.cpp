#include <iostream>
#include <mtao/types.hpp>
#include <mtao/eigen/stack.h>
#include <mtao/geometry/mesh/shapes/tube.hpp>

int main(int argc, char* argv[]) {

    int M = 4;
    int O = 10;
    int N = 50;
    mtao::ColVecs3d V(3,N);
    mtao::VecXd theta = mtao::VecXd::LinSpaced(N+1,0,M * 2*M_PI).head(N);

    mtao::ColVecs3d CS = mtao::eigen::hstack(theta.array().cos(),theta.array().sin(),mtao::VecXd::Zero(N)).transpose();
    V.topRows(2) = CS;
    V.row(2) = theta.transpose() / (2 * M_PI * M);

    {
        auto [nV,nF] = mtao::geometry::mesh::shapes::disc<true>(mtao::Vec3d::Unit(0).eval(),.1);
        //auto [nV,nF] = mtao::geometry::mesh::shapes::disc<true>(mtao::Vec3d::Unit(0).eval(),mtao::Vec3d::Unit(1).eval());
        //auto [nV,nF] = mtao::geometry::mesh::shapes::capped_tube(V,.1,O);
        for(int i = 0; i < nV.cols(); ++i) {
            std::cout << "v " << nV.col(i).transpose() << std::endl;
        }
        for(int i = 0; i < nF.cols(); ++i) {
            std::cout << "f " << (nF.col(i).array()+1).transpose() << std::endl;
        }
    }

    return 0;

}
