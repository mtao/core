#pragma once
#include <Eigen/Core>
#include <numeric>

namespace mtao::linear_algebra {
// M is matrix, B is initial vector, Q is the basis for the basis
// N is the dimension of the subspace

namespace internal {
    template<typename Derived, typename BDerived, typename QDerived, typename HDerived>
    auto arnoldi(const Eigen::MatrixBase<Derived> &M, const Eigen::MatrixBase<BDerived> &b, Eigen::PlainObjectBase<QDerived> &Q, Eigen::PlainObjectBase<HDerived> &H) {
        const int N = H.rows() - 1;
        using Scalar = typename Derived::Scalar;
        constexpr Scalar eps = std::numeric_limits<Scalar>::epsilon();
        using Vec = Eigen::Matrix<Scalar, Derived::RowsAtCompileTime, 1>;


        Q.setZero();
        H.setZero();
        Vec v;

        Q.col(0) = b.normalized();
        for (int j = 0; j <= N; ++j) {

            auto q = Q.col(j);
            v = M * q;
            for (int k = 0; k <= j; ++k) {
                auto l = Q.col(k);
                const Scalar s = l.dot(v);
                H(k, j) = s;
                v -= s * l;
            }
            if (j < N) {
                const Scalar n = H(j + 1, j) = v.norm();
                if (n > eps) {
                    v /= n;
                    Q.col(j + 1) = v;
                    continue;
                }
            }
            return;
        }
    }
}// namespace internal

template<typename Derived, typename BDerived>
auto arnoldi(const Eigen::MatrixBase<Derived> &M, const Eigen::MatrixBase<BDerived> &B, int N) {
    assert(M.rows() == M.cols());

    using Scalar = typename Derived::Scalar;
    using Mat = Eigen::Matrix<Scalar, Derived::RowsAtCompileTime, Eigen::Dynamic>;
    using DMat = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;

    Mat Q(M.rows(), N);
    DMat H(N, N);

    internal::arnoldi(M, B, Q, H);
    return std::make_tuple(Q, H);
}
template<int N, typename Derived, typename BDerived>
auto arnoldi(const Eigen::MatrixBase<Derived> &M, const Eigen::MatrixBase<BDerived> &B) {
    assert(M.rows() == M.cols());

    using Scalar = typename Derived::Scalar;
    using Mat = Eigen::Matrix<Scalar, Eigen::Dynamic, N>;
    using HMat = Eigen::Matrix<Scalar, N, N>;

    Mat Q(M.rows(), N);
    HMat H(N, N);
    internal::arnoldi(M, B, Q, H);
    return std::make_tuple(Q, H);
}
template<typename Derived>
auto arnoldi(const Eigen::MatrixBase<Derived> &M, int N) {
    using Scalar = typename Derived::Scalar;
    auto B = Eigen::Matrix<Scalar, Derived::RowsAtCompileTime, 1>::Random(M.rows());
    return arnoldi(M, B, N);
}
template<int N, typename Derived>
auto arnoldi(const Eigen::MatrixBase<Derived> &M) {
    using Scalar = typename Derived::Scalar;
    auto B = Eigen::Matrix<Scalar, Derived::RowsAtCompileTime, 1>::Random(M.rows());
    return arnoldi<N>(M, B);
}
}// namespace mtao::linear_algebra
