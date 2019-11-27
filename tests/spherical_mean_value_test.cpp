#include <mtao/geometry/interpolation/mean_value.hpp>
#include <mtao/geometry/solid_angle.hpp>
#include <mtao/types.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    using namespace mtao::geometry;
    using namespace mtao::geometry::interpolation;

    mtao::Mat3d A = mtao::Mat3d::Identity();
    mtao::Mat3d B = A;
    mtao::Mat3d C = A;
    mtao::Mat3d D = A;
    B.col(0).setZero();
    C.col(1).setZero();
    D.col(2).setZero();


    mtao::ColVecs3d V(3,4);
    V << 0,1,0,0, 0,0,1,0, 0,0,0,1;
    mtao::ColVecs3i F(3,4);

    F << 1,1,3,2, 2,0,0,0, 3,2,1,3;

    std::cout << "V" << std::endl;
    std::cout << V << std::endl;

    std::cout << "F" << std::endl;
    std::cout << F << std::endl;

    std::cout << solid_angle_mesh(V,F,mtao::Vec3d(.1,.1,.1)) << std::endl;


    /*
    std::cout << spherical_mean_value_unnormalized(A,mtao::Vec3d::Zero()) << std::endl;
    for(int i = 0; i < 5; ++i) {
        auto v = mtao::Vec3d::Random().eval();
        std::cout << v.transpose() << " ( " << ((v.sum() > 1)?"Neg":"Pos") << " ) => " 
            << spherical_mean_value_unnormalized(A,v).transpose()   << ": "
            << spherical_mean_value_unnormalized(B,v).transpose()   << ": "
            << spherical_mean_value_unnormalized(C,v).transpose()   << ": "
            << spherical_mean_value_unnormalized(D,v).transpose()  
            << std::endl;
    }
    */

    std::cout << std::endl;
    std::cout << std::endl;
    auto run = [&](auto&& v) {

        /*
        auto a =  spherical_mean_value_unnormalized(A,v);
        auto b =  spherical_mean_value_unnormalized(B,v);
        auto c =  spherical_mean_value_unnormalized(C,v);
        auto d =  spherical_mean_value_unnormalized(D,v); 
        std::cout << v.transpose() << " ( " << ((v.sum() > 1)?"Neg":"Pos") << " ) => " 
            << a.transpose() << " : "
            << b.transpose() << " : "
            << c.transpose() << " : "
            << d.transpose() << " : "
            << std::endl;
        */
        auto P = spherical_mean_value(V,F,v);
        std::cout << P << " => " << P.sum() << std::endl;

    };

    run(mtao::Vec3d::Constant(.1).eval());
    std::cout << std::endl << std::endl << std::endl;
    run(mtao::Vec3d::Constant(.1));
    run(mtao::Vec3d(.9,.01,.01));
    run(mtao::Vec3d(.01,.9,.01));
    run(mtao::Vec3d(.01,.01,.9));
    run(mtao::Vec3d(.01,.01,.01));
}
