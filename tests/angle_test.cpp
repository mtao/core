#include <iostream>
#include <mtao/geometry/trigonometry.hpp>


int main() {
    mtao::ColVecs2d V(2,5);
    V.col(0) = mtao::Vec2d(0.0,0.0);
    V.col(1) = mtao::Vec2d(1.0,0.0);
    V.col(2) = mtao::Vec2d(1.0,1.0);
    V.col(3) = mtao::Vec2d(0.0,1.0);
    V.col(4) = mtao::Vec2d(0.5,0.5);

    std::cout << mtao::geometry::trigonometry::interior_angle_sum(5) << " => "
        << mtao::geometry::trigonometry::exterior_angle_sum(5) << std::endl;
    std::cout << mtao::geometry::trigonometry::angle_sum(V,{0,1,2,3,4})<< " "
        << mtao::geometry::trigonometry::angle_sum(V,{4,3,2,1,0})<< std::endl;
    std::cout << mtao::geometry::trigonometry::interior_angle_sum(4) << " => "
        << mtao::geometry::trigonometry::exterior_angle_sum(4) << std::endl;
    std::cout << mtao::geometry::trigonometry::angle_sum(V,{0,1,2,3})<< " "
        << mtao::geometry::trigonometry::angle_sum(V,{3,2,1,0})<< std::endl;
    std::cout << mtao::geometry::trigonometry::interior_angle_sum(3) << " => "
        << mtao::geometry::trigonometry::exterior_angle_sum(3) << std::endl;
    std::cout << mtao::geometry::trigonometry::angle_sum(V,{0,1,2}) << " "
        << mtao::geometry::trigonometry::angle_sum(V,{0,2,1}) << std::endl;
}
