#include <mtao/geometry/barycentric.hpp>
#include <iostream>


using Triangle = mtao::Matrix<double, 3, 3>;
using Tetrahedron = mtao::Matrix<double, 3, 4>;
using Vecs3 = mtao::ColVectors<double, 3>;
using Vec3 = mtao::Vector<double, 3>;
using Vec4 = mtao::Vector<double, 4>;


void tri() {

    Triangle T;
    T.col(0) = Vec3(0.0, 0.0, 0.0);
    T.col(1) = Vec3(1.0, 0.0, 0.0);
    T.col(2) = Vec3(0.0, 1.0, 0.0);


    mtao::ColVectors<double, 3> B(3, 6);
    B.col(0) = Vec3(1.0, 0.0, 0.0);
    B.col(1) = Vec3(0.0, 1.0, 0.0);
    B.col(2) = Vec3(0.0, 0.0, 1.0);
    B.col(3) = Vec3(0.0, 1.0, 1.0);
    B.col(4) = Vec3(1.0, 0.0, 1.0);
    B.col(5) = Vec3(1.0, 1.0, 0.0);


    mtao::ColVectors<double, 3> P(3, B.cols());

    for (int i = 0; i < P.cols(); ++i) {
        P.col(i) = T * B.col(i) / B.col(i).sum();
    }
    std::cout << P << std::endl;

    for (int i = 0; i < P.cols(); ++i) {
        std::cout << i << ") " << mtao::geometry::barycentric_simplicial(T, P.col(i).eval()).transpose() << std::endl;
    }
}


void tet() {

    //Triangle T;
    Tetrahedron T;
    T.col(0) = Vec3(0.0, 0.0, 0.0);
    T.col(1) = Vec3(1.0, 0.0, 0.0);
    T.col(2) = Vec3(0.0, 1.0, 0.0);
    T.col(3) = Vec3(0.0, 0.0, 1.0);


    mtao::ColVectors<double, 4> B(4, 8);
    B.col(0) = Vec4(1.0, 0.0, 0.0, 0.0);
    B.col(1) = Vec4(0.0, 1.0, 0.0, 0.0);
    B.col(2) = Vec4(0.0, 0.0, 1.0, 0.0);
    B.col(3) = Vec4(0.0, 0.0, 0.0, 1.0);
    B.col(4) = Vec4(0.0, 1.0, 1.0, 1.0);
    B.col(5) = Vec4(1.0, 0.0, 1.0, 1.0);
    B.col(6) = Vec4(1.0, 1.0, 0.0, 1.0);
    B.col(7) = Vec4(1.0, 1.0, 1.0, 0.0);


    mtao::ColVectors<double, 3> P(3, B.cols());

    for (int i = 0; i < P.cols(); ++i) {
        P.col(i) = T * B.col(i) / B.col(i).sum();
    }
    std::cout << P << std::endl;

    for (int i = 0; i < P.cols(); ++i) {
        std::cout << i << ") " << mtao::geometry::barycentric_simplicial(T, P.col(i).eval()).transpose() << std::endl;
    }
}
int main() {
    tri();
    tet();
    return 0;
}
