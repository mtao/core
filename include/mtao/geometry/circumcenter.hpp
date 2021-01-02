#ifndef CIRCUMCENTER_H
#define CIRCUMCENTER_H
#include <mtao/types.h>

#include <Eigen/Dense>

namespace mtao {
namespace geometry {

    template<typename Derived>
    auto circumcenter_spd(const Eigen::MatrixBase<Derived> &V) {
        // 2 V.dot(C) = sum(V.colwise().squaredNorm()).transpose()

        // probably dont really need this temporary
        auto m = (V.rightCols(V.cols() - 1).colwise() - V.col(0)).eval();
        auto A = (2 * m.transpose() * m).eval();
        auto b = m.colwise().squaredNorm().transpose().eval();
        A.llt().solveInPlace(b);

        auto c = (V.col(0) + m * b).eval();
        return c;
    }
    template<typename Derived>
    auto circumcenter_spsd(const Eigen::MatrixBase<Derived> &V) {
        mtao::MatrixX<typename Derived::Scalar> A(V.cols() + 1, V.cols() + 1);
        A.setConstant(1);
        A(V.cols(), V.cols()) = 0;
        auto m = V.transpose() * V;
        ;
        A.topLeftCorner(V.cols(), V.cols()) = 2 * m;
        mtao::VectorX<typename Derived::Scalar> b(V.cols() + 1);
        b.setConstant(1);
        b.topRows(m.cols()) = V.colwise().squaredNorm().transpose();

        auto x = A.colPivHouseholderQr().solve(b).eval();

        return (V * x.topRows(V.cols())).eval();
    }
    template<typename Derived>
    auto circumcenter(const Eigen::MatrixBase<Derived> &V) {
        return circumcenter_spd(V);
    }

    template<typename VertexDerived, typename SimplexDerived>
    auto circumcenters(const Eigen::MatrixBase<VertexDerived> &V,
                       const Eigen::MatrixBase<SimplexDerived> &S) {
        constexpr static int E = VertexDerived::RowsAtCompileTime;// embed dim
        constexpr static int N =
          SimplexDerived::ColsAtCompileTime;// number of elements
        constexpr static int D = SimplexDerived::RowsAtCompileTime;// simplex dim
        using Scalar = typename VertexDerived::Scalar;

        Eigen::Matrix<Scalar, E, N> C(V.rows(), S.cols());

        Eigen::Matrix<Scalar, E, D> v(V.rows(), S.rows());
#pragma omp parallel for private(v)
        for (int i = 0; i < S.cols(); ++i) {
            auto s = S.col(i);
            for (int j = 0; j < S.rows(); ++j) {
                v.col(j) = V.col(s(j));
            }
            C.col(i) = circumcenter(v);
        }
        return C;
    }
}// namespace geometry
}// namespace mtao
#endif// CIRCUMCENTER_H
