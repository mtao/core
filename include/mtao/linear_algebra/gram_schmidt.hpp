#pragma once
#include <Eigen/Core>

namespace mtao::linear_algebra {
    // the gram schmidt algorithm with a threshold for when 0-eigenvalues are found
    template <typename Derived>
        void gram_schmidt_in_place(Eigen::PlainObjectBase<Derived>& M) {
            using Scalar = typename Derived::Scalar;
            constexpr Scalar eps = std::numeric_limits<Scalar>::epsilon();
            for(int j = 0; j < M.cols(); ++j) {
                auto u = M.col(j);
                Scalar n = u.squaredNorm();
                for(int k = 0; k < j; ++k) {
                    auto v = M.col(k);
                    u -= v.dot(u) * v;
                }
                if(u.squaredNorm() < eps * n) {
                    u.setZero();
                } else {
                    u.normalize();
                }
            }
        }
    template <typename Derived>
        auto gram_schmidt(const Eigen::MatrixBase<Derived>& M) {
            typename Derived::PlainMatrix O = M;
            gram_schmidt_in_place(O);
            return O;
        }
}
