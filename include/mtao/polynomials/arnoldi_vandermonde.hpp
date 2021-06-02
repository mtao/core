#pragma once
#include <iomanip>
#include "mtao/types.hpp"
#include <iostream>
#include "mtao/eigen/shape_checks.hpp"
#include <numeric>

namespace mtao::polynomials {
// M is matrix, B is initial vector, Q is the basis for the basis
// N is the dimension of the subspace


// D = polynomial degree
template<typename T, int D = Eigen::Dynamic>
struct ArnoldiPolynomial {
    constexpr static bool IsDynamic = D == Eigen::Dynamic;
    using Scalar = T;

    using Vector = std::conditional_t<IsDynamic, mtao::VectorX<T>, mtao::Vector<T, D>>;
    using CoeffVector = std::conditional_t<IsDynamic, mtao::VectorX<T>, mtao::Vector<T, D + 1>>;
    using HessenbergeMatrix = std::conditional_t<IsDynamic, mtao::MatrixX<T>, Eigen::Matrix<T, D + 1, D>>;

    template<eigen::concepts::VecCompatible XType, eigen::concepts::VecCompatible FType>
    ArnoldiPolynomial(const XType &X, const FType &F, int n = 0);

    T operator()(T v) const;

    template<eigen::concepts::VecCompatible XType>
    std::decay_t<typename XType::EvalReturnType> operator()(const XType &v) const;


    int degree() const { return _H.cols(); }

    CoeffVector _C;//coefficients
    HessenbergeMatrix _H;//hessenberg matrix
};
template<typename Vec, typename Vec2>
ArnoldiPolynomial(const Vec &, const Vec2 &, int) -> ArnoldiPolynomial<typename Vec::Scalar, Vec::RowsAtCompileTime>;


template<typename T, int D>
template<eigen::concepts::VecCompatible XType, eigen::concepts::VecCompatible FType>
ArnoldiPolynomial<T, D>::ArnoldiPolynomial(const XType &X, const FType &F, int n) : _C(IsDynamic ? n + 1 : D + 1), _H(HessenbergeMatrix::Zero((IsDynamic ? n : D) + 1, (IsDynamic ? n : D))) {

    const int M = X.size();
    const T Msqrt = std::sqrt<T>(M).real();
    mtao::ColVectors<T, XType::RowsAtCompileTime> Q(M, degree() + 1);
    Q.setZero();
    Q.col(0).setOnes();
    auto XDiag = X.asDiagonal();
    // for each degree,
    for (int k = 0; k < degree(); ++k) {
        auto q = Q.col(k + 1);
        q = XDiag * Q.col(k);
        for (int j = 0; j <= k; ++j) {
            auto qj = Q.col(j);
            const Scalar h = _H(j, k) = qj.dot(q) / M;
            //std::cout << q.transpose() << "\n"
            //          << qj.transpose() << std::endl;
            q = q - h * qj;
        }
        const T h = _H(k + 1, k) = q.norm() / Msqrt;
        //std::cout << std::setprecision(15) << h << std::endl;
        q /= h;
        //std::cout << Q.leftCols(k + 2) << std::endl;
        //std::cout << _H << std::endl
        //          << std::endl;
    }
    //std::cout << _H << std::endl;
    if (Q.rows() == Q.cols()) {
        _C.noalias() = Q.lu().solve(F);
    } else {
        _C.noalias() = Q.householderQr().solve(F);
    }
}


template<typename T, int D>
T ArnoldiPolynomial<T, D>::operator()(T v) const {
    // just stuff it in a 1-row-vector and move on
    auto vec = mtao::Vector<T, 1>::Constant(v);
    auto ret = (*this)(vec);
    return ret(0);
}

template<typename T, int D>
template<eigen::concepts::VecCompatible XType>
std::decay_t<typename XType::EvalReturnType> ArnoldiPolynomial<T, D>::operator()(const XType &X) const {

    mtao::ColVectors<T, XType::RowsAtCompileTime> W(X.size(), degree() + 1);

    W.setZero();
    W.col(0).setOnes();
    auto XDiag = X.asDiagonal();
    for (int k = 0; k < degree(); ++k) {
        auto w = W.col(k + 1);
        w = XDiag * W.col(k);
        for (int j = 0; j <= k; ++j) {
            auto wj = W.col(j);
            w = w - _H(j, k) * wj;
        }
        w /= _H(k + 1, k);
    }

    return W * _C;
}
}// namespace mtao::polynomials

