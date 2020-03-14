#pragma once
#include <Eigen/Core>
#include <numeric>

namespace mtao::linear_algebra {
    // M is matrix, B is initial vector, Q is the basis for the basis
    // N is the dimension of the subspace

    namespace internal {
        template <typename Derived, typename BDerived, typename VDerived, typename TDerived>
            auto lanczos(const Eigen::MatrixBase<Derived>& M, const Eigen::MatrixBase<BDerived>& v1, Eigen::PlainObjectBase<VDerived>& V, Eigen::PlainObjectBase<TDerived>& T) {
                const int N = T.rows();
                using Scalar = typename Derived::Scalar;
                constexpr double eps = std::numeric_limits<Scalar>::epsilon();
                using Vec = Eigen::Matrix<Scalar,Derived::RowsAtCompileTime, 1>;

                V.setZero();
                T.setZero();

                V.col(0) = v1;
                Vec w;

                {// first iteration
                    auto v = V.col(0);
                    w = M * v;
                    const Scalar& a = T(0,0) = w.dot(v);

                    w -= a * v;
                }
                for(int j = 1; j < N; ++j) {
                    const Scalar& b = T(j,j-1) = T(j-1,j) = w.norm();
                    if(b < eps) {
                        return;
                    }
                    auto vjm = V.col(j-1);
                    auto vj = V.col(j) = w/b;
                    w = M * vj;
                    const Scalar& a = T(j,j) = w.dot(vj);
                    w -= a * vj + b * vjm;
                }
            }
    }

    template <typename Derived, typename BDerived>
        auto lanczos(const Eigen::MatrixBase<Derived>& M, const Eigen::MatrixBase<BDerived>& B, int N) {
            assert(M.rows() == M.cols());

            using Scalar = typename Derived::Scalar;
            using Mat = Eigen::Matrix<Scalar,Derived::RowsAtCompileTime,Eigen::Dynamic>;
            using DMat = Eigen::Matrix<Scalar,Eigen::Dynamic,Eigen::Dynamic>;

            Mat Q(M.rows(),N);
            DMat H(N,N);

            internal::lanczos(M,B,Q,H);
            return std::make_tuple(Q,H);
        }
    template <int N, typename Derived, typename BDerived>
        auto lanczos(const Eigen::MatrixBase<Derived>& M, const Eigen::MatrixBase<BDerived>& B) {
            assert(M.rows() == M.cols());

            using Scalar = typename Derived::Scalar;
            using Mat = Eigen::Matrix<Scalar,Eigen::Dynamic,N>;
            using HMat = Eigen::Matrix<Scalar,N,N>;

            Mat Q(M.rows(),N);
            HMat H(N,N);
            internal::lanczos(M,B,Q,H);
            return std::make_tuple(Q,H);
        }
    template <typename Derived>
        auto lanczos(const Eigen::MatrixBase<Derived>& M, int N) {
            using Scalar = typename Derived::Scalar;
            auto B = Eigen::Matrix<Scalar,Derived::RowsAtCompileTime, 1>::Random(M.rows()) ;
            return lanczos(M,B,N);
        }
    template <int N, typename Derived>
        auto lanczos(const Eigen::MatrixBase<Derived>& M) {
            using Scalar = typename Derived::Scalar;
            auto B = Eigen::Matrix<Scalar,Derived::RowsAtCompileTime, 1>::Random(M.rows()) ;
            return lanczos<N>(M,B);
        }
}
