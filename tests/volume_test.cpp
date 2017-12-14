#include <iostream>
#include "mtao/geometry/volume.h"
#include "mtao/types.h"



int main() {

    mtao::MatrixX<float> m(2,2);

    m.col(0) = mtao::Vec2f(0.0,4.0);
    m.col(1) = mtao::Vec2f(3.0,0.0);
    std::cout << "5 = " << mtao::geometry::volume(m) << std::endl;

    //ccw is positive
    m.resize(2,3);
    m.col(0) = mtao::Vec2f(0.0,0.0);
    m.col(1) = mtao::Vec2f(3.0,0.0);
    m.col(2) = mtao::Vec2f(0.0,4.0);

    std::cout << "6 = " << mtao::geometry::volume(m) << std::endl;

    //cw is negative
    m.col(2) = mtao::Vec2f(3.0,0.0);
    m.col(1) = mtao::Vec2f(0.0,4.0);
    std::cout << "-6 = " << mtao::geometry::volume(m) << std::endl;

    m.resize(3,3);
    m.col(0) = mtao::Vec3f(0.0,0.0,0.0);
    m.col(1) = mtao::Vec3f(3.0,0.0,0.0);
    m.col(2) = mtao::Vec3f(0.0,4.0,0.0);
    std::cout << "6 = " << mtao::geometry::volume(m) << std::endl;

    m.resize(3,4);
    m.col(0) = mtao::Vec3f(0.0,0.0,0.0);
    m.col(1) = mtao::Vec3f(3.0,0.0,0.0);
    m.col(2) = mtao::Vec3f(0.0,4.0,0.0);
    m.col(3) = mtao::Vec3f(0.0,4.0,5.0);
    std::cout << "10 = " << mtao::geometry::volume(m) << std::endl;


}
