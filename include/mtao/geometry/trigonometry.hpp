#pragma once

#include <mtao/types.hpp>
#include <iostream>

namespace mtao::geometry::trigonometry {

    template <typename S>
    S angle_clamp_positive(S ang) {
        while(ang > 2* M_PI) {
            ang -= 2 * M_PI;
        } 
        while(ang < 0) {
            ang += 2*M_PI;
        }
        return ang;
    }
    template <typename S>
    S angle_clamp_pi(S ang) {
        while(ang > M_PI) {
            ang -= 2 * M_PI;
        } 
        while(ang < -M_PI) {
            ang += 2*M_PI;
        }
        return ang;
    }
    template <typename S>
        S angle_scalar(S x, S y) {
            return angle_clamp_positive(std::atan2(y,x));
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
            R.noalias() = R.unaryExpr(std::ptr_fun(angle_clamp_positive<S>));
            return R;
        }

    template <typename T = double>
        T interior_angle_sum(int size) {
            return  M_PI * ( size - 2);
        }
    template <typename T = double>
        T exterior_angle_sum(int size) {
            return  M_PI * ( size + 2);
        }
    template <typename Derived,typename BeginIt, typename EndIt>
        typename Derived::Scalar angle_sum(const Eigen::MatrixBase<Derived>& V, const BeginIt& beginit, const EndIt& endit) {
            using S = typename Derived::Scalar;
            S ang_sum = 0;
            for(auto it = beginit; it != endit; ++it) {
                auto it1 = it;
                it1++;
                if(it1 == endit) { it1 = beginit; }
                auto it2 = it1;
                it2++;
                if(it2 == endit) { it2 = beginit; }
                auto a = V.col(*it);
                auto b = V.col(*it1);
                auto c = V.col(*it2);
                double ang = angle(c-b,a-b)(0);
                ang_sum += ang;
            }

            return ang_sum;
        }
    template <typename Derived,typename Container>
        auto angle_sum(const Eigen::MatrixBase<Derived>& V, const Container& C) {
            return angle_sum(V,C.begin(),C.end());
        }
    template <typename Derived,typename T = int>
        auto angle_sum(const Eigen::MatrixBase<Derived>& V, const std::initializer_list<T>& C) {
            return angle_sum(V,C.begin(),C.end());
        }

    template <typename Derived,typename BeginIt, typename EndIt>
        bool clockwise_cell(const Eigen::MatrixBase<Derived>& V, const BeginIt& beginit, const EndIt& endit) {

            using S = typename Derived::Scalar;
            int size =  std::distance(beginit,endit);
            S ang = angle_sum(V,beginit,endit);
            S expected=  interior_angle_sum<S>(size);
            bool ret = std::abs(ang - expected) < 1e-5;
            
            if(ret) {
                assert(std::abs(ang-exterior_angle_sum<S>(size)) > 1e-5);
            } else {
                assert(std::abs(ang-exterior_angle_sum<S>(size)) < 1e-5);
            }
            return ret;
        }
    template <typename Derived,typename Container>
        auto clockwise_cell(const Eigen::MatrixBase<Derived>& V, const Container& C) {
            return clockwise_cell(V,C.begin(),C.end());
        }
    template <typename Derived,typename T = int>
        auto clockwise_cell(const Eigen::MatrixBase<Derived>& V, const std::initializer_list<T>& C) {
            return clockwise_cell(V,C.begin(),C.end());
        }
}
