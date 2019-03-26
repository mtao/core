#pragma once

#include <mtao/types.hpp>

namespace mtao::geometry::trigonometry {

    template <typename S>
    S positive_angle_clamp(S ang) {
        while(ang > 2* M_PI) {
            ang -= 2 * M_PI;
        } 
        while(ang < 0) {
            ang += 2*M_PI;
        }
        return ang;
    }
    template <typename S>
        S angle_scalar(S x, S y) {
            return positive_angle_clamp(std::atan2(y,x));
        }

    template <typename Derived>
        auto angle(const Eigen::MatrixBase<Derived>& P) {
            using S = typename Derived::Scalar;
            Eigen::Matrix<S,1,Derived::ColsAtCompileTime> A(1,P.cols());

            A = P.row(0).binaryExpr(P.row(1), std::ptr_fun(angle_scalar<S>));
            return A;
        }
    template <typename Derived, typename Derived1>
        auto angle(const Eigen::MatrixBase<Derived>& A, const Eigen::MatrixBase<Derived1>& B) {
            using S = typename Derived::Scalar;
            auto R = (angle(B) - angle(A)).eval();
            R.noalias() = R.unaryExpr(std::ptr_fun(positive_angle_clamp<S>));
            return R;
        }

}
