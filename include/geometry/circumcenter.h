#ifndef CIRCUMCENTER_H
#define CIRCUMCENTER_H
#include <Eigen/Dense>

namespace mtao { namespace geometry {
    template <typename Derived>
auto circumcenter( Eigen::MatrixBase<Derived> & V) {

    //2 V.dot(C) = sum(V.colwise().squaredNorm()).transpose()

    //probably dont really need this temporary
    auto m = (V.rightCols(V.cols()-1).colwise() - V.col(0)).eval();
    auto A = (2 * m.transpose() * m).eval();
    auto b = m.colwise().squaredNorm().transpose().eval();
    A.ldlt().solveInPlace(b);
    return (V.col(0) + m*b).eval();

}
}}
#endif//CIRCUMCENTER_H
