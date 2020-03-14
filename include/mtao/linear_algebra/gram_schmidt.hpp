#pragma once
#include <Eigen/Core>

namespace mtao::linear_algebra {
    template <typename Derived>
        void gram_schmidt_in_place(const Eigen::MatrixBase<Derived>& M) {
            for(int j = 0; j < M.cols(); ++j) {
                auto u = M.col(j);
                for(int k = 0; k < j; ++k) {
                    auto v = M.col(k);
                    u -= v.dot(u) * v;
                }
                u.normalize();
            }
        }
    template <typename Derived>
        auto gram_schmidt(const Eigen::MatrixBase<Derived>& M) {
            typename Derived::PlainMatrix O = M;
            gram_schmidt_in_place(O);
            return O;
        }
}
