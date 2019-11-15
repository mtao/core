#include <mtao/geometry/barycentric_matrix.hpp>
#include <iostream>


using Triangle = mtao::Matrix<double,3,3>;
using Tetrahedron = mtao::Matrix<double,3,4>;
using Vecs3 = mtao::ColVectors<double,3>;
using Vec3 = mtao::Vector<double,3>;
using Vec4 = mtao::Vector<double,4>;


void tri() {

    Triangle T;
    T.col(0) = Vec3(0.0,0.0,0.0);
    T.col(1) = Vec3(1.0,0.0,0.0);
    T.col(2) = Vec3(0.0,1.0,0.0);



    mtao::ColVectors<double,3> B(3,6);
    B.col(0) = Vec3(1.0,0.0,0.0);
    B.col(1) = Vec3(0.0,1.0,0.0);
    B.col(2) = Vec3(0.0,0.0,1.0);
    B.col(3) = Vec3(0.0,1.0,1.0);
    B.col(4) = Vec3(1.0,0.0,1.0);
    B.col(5) = Vec3(1.0,1.0,0.0);

    for(int i = 0; i < B.cols(); ++i) {
        B.col(i) = B.col(i) / B.col(i).sum();
    }
    mtao::ColVecs3i F(3,3);
    F << 0,1,2,3,4,5,6,7,8;

    mtao::VecXi FI(6); 
    FI << 0,0,1,1,2,2;


    std::cout << mtao::geometry::barycentric_matrix(9,F,FI,B) << std::endl;

}


void tet() {

    //Triangle T;
    Tetrahedron T;
    T.col(0) = Vec3(0.0,0.0,0.0);
    T.col(1) = Vec3(1.0,0.0,0.0);
    T.col(2) = Vec3(0.0,1.0,0.0);
    T.col(3) = Vec3(0.0,0.0,1.0);




    mtao::ColVectors<double,4> B(4,8);
    B.col(0) = Vec4(1.0,0.0,0.0,0.0);
    B.col(1) = Vec4(0.0,1.0,0.0,0.0);
    B.col(2) = Vec4(0.0,0.0,1.0,0.0);
    B.col(3) = Vec4(0.0,0.0,0.0,1.0);
    B.col(4) = Vec4(0.0,1.0,1.0,1.0);
    B.col(5) = Vec4(1.0,0.0,1.0,1.0);
    B.col(6) = Vec4(1.0,1.0,0.0,1.0);
    B.col(7) = Vec4(1.0,1.0,1.0,0.0);


    mtao::ColVecs4i F(4,4);
    F << 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15;

    mtao::VecXi FI(8); 
    FI << 0,0,1,1,2,2,3,3,4,4;
    mtao::ColVectors<double,4> P(4,B.cols());

    for(int i = 0; i < B.cols(); ++i) {
        B.col(i) = B.col(i) / B.col(i).sum();
    }

    std::cout << mtao::geometry::barycentric_matrix(16,F,FI,B) << std::endl;



}
int main() {
    tri();
    tet();
    return 0;
}
