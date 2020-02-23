#pragma once

#include <mtao/types.hpp>

namespace mtao::geometry::trigonometry {

    // pushes an angle to the range [0,2*M_PI)
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
    // pushes an angle to the range [-M_PI,M_PI)
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
    // Returns the angle between a vector and [1,0]^*
    template <typename S>
        S angle_scalar(S x, S y) {
            return angle_clamp_positive(std::atan2(y,x));
        }

    // Returns the angle for each vector in a ColVector and returns a rowvector
    template <typename Derived>
        auto angle(const Eigen::MatrixBase<Derived>& P) {
            using S = typename Derived::Scalar;
            Eigen::Matrix<S,1,Derived::ColsAtCompileTime> A(1,P.cols());

#ifdef TODO_CHECK_WHEN_THIS_IS_OK
            A = P.row(0).binaryExpr(P.row(1), std::function(angle_scalar<S>));
#else
            auto a = P.row(0);
            auto b = P.row(1);
            for(int i = 0; i < P.cols(); ++i) {
                A(i) = angle_scalar<S>(a(i),b(i));
            }
#endif
            return A;
        }

    // Returns the angle of between each col from A to B clockwise
    template <typename Derived, typename Derived1>
        auto angle(const Eigen::MatrixBase<Derived>& A, const Eigen::MatrixBase<Derived1>& B) {
            auto R = (angle(B) - angle(A)).eval();
#ifdef TODO_CHECK_WHEN_THIS_IS_OK
            using S = typename Derived::Scalar;
            R.noalias() = R.unaryExpr(std::function(angle_clamp_positive<S>));
#else
            for(int i = 0; i < R.size(); ++i) {
                R(i) = angle_clamp_positive(R(i));
            }
#endif
            return R;
        }

    // The interior angle of a polygon with size vertices/edges
    template <typename T = double>
        T interior_angle_sum(int size) {
            return  M_PI * ( size - 2);
        }
    // The exterior angle of a polygon with size vertices/edges
    template <typename T = double>
        T exterior_angle_sum(int size) {
            return  M_PI * ( size + 2);
        }

    // The sum of angles generated by a collection of points.
    // Should be equal to interior_angle_sum(n) or exterior_angle_sum(n)
    // for n = std::distance(beginit,endit)
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

            /*
            //counteract size when we have degenerate cells
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
            }
            */
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
