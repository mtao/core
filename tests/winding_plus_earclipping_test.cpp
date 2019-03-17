#include "mtao/geometry/mesh/earclipping.hpp"
#include "mtao/geometry/winding_number.hpp"
#include <iostream>


int main() {
    mtao::ColVectors<double,2> V(2,5);
    V.col(0) = mtao::Vec2d(0.0,0.0);
    V.col(1) = mtao::Vec2d(1.0,0.0);
    V.col(2) = mtao::Vec2d(1.0,1.0);
    V.col(3) = mtao::Vec2d(0.0,1.0);
    V.col(4) = mtao::Vec2d(0.5,0.5);
    std::vector<int> F = {0,1,2,3,4};
    int NI=10, NJ=10;
    for(int i = 0; i <= NI; ++i) {
        for(int j = 0; j <= NJ; ++j) {
            mtao::Vec2d p(double(i)/NI,double(j)/NJ);
            //std::cout << mtao::geometry::winding_number(V,F,p) << " ";
            //std::cout << (mtao::geometry::winding_number(V,F,p)>1?'o':'.') << " ";
            std::cout << (mtao::geometry::interior_winding_number(V,F,p)?'o':'.') << " ";
        }
        std::cout << std::endl;
    }
    std::cout << mtao::geometry::mesh::earclipping(V,F) << std::endl;
    std::cout << mtao::geometry::mesh::earclipping(V,{0,1,2,3}) << std::endl;
    std::cout << mtao::geometry::mesh::earclipping(V,{F,F}) << std::endl;
    return 0;
}
