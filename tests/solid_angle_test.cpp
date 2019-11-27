#include <mtao/geometry/solid_angle.hpp>
#include <mtao/types.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    using namespace mtao::geometry;

    mtao::Mat3d A = mtao::Mat3d::Identity();

    std::cout << solid_angle(A) << " " << solid_angle(A.col(0),A.col(1),A.col(2)) << std::endl;
    for(int i = 0; i < 5; ++i) {
        auto v = mtao::Vec3d::Random().eval();
        std::cout << solid_angle(A,v) << " " << solid_angle(A.col(0),A.col(1),A.col(2),v) << std::endl;
    }


}
