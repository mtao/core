#pragma once
#include <cmath>
#include "mtao/types.hpp"
#include "mtao/eigen/shape_checks.hpp"

namespace mtao::geometry::interpolation {


    template <typename RBFunc, typename PointsType, typename VecType>
        auto  radial_basis_function(RBFunc&& f, const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v, typename PointsType::Scalar radius, bool normalize=false) -> mtao::VectorX<typename PointsType::Scalar> {
            static_assert(std::is_same_v<typename PointsType::Scalar,typename VecType::Scalar>);
            eigen::col_check<1>(v);

            using Scalar = typename PointsType::Scalar;
            mtao::VectorX<Scalar> R(P.cols());
            mtao::RowVectorX<Scalar> D = (P.colwise() - v).colwise().norm();
            
            R = D.unaryExpr(f).transpose() / radius;
            if(normalize) {
                R /= R.sum();
            }
            return R;
        }
    template <typename PointsType, typename VecType>
        auto  gaussian_rbf(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v, typename PointsType::Scalar radius, bool normalize=false) {
            using Scalar = typename PointsType::Scalar;
            auto R = radial_basis_function(
                    [](Scalar v) -> Scalar { return std::exp(-.5 * v * v); },
                    P,v,radius,normalize);
            if(!normalize) {
                R /= (std::sqrt(2 * M_PI) * radius);
            }
            return R;
        }

    template <typename PointsType, typename VecType>
        auto  spline_gaussian_rbf(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v, typename PointsType::Scalar radius, bool normalize=false) {
            using Scalar = typename PointsType::Scalar;
            auto R = radial_basis_function(
                    [](Scalar v) -> Scalar { 
                    if(v < 1) {
                    return 1 - 1.5 * v*v + .75 * v*v*v;
                    } else if(v < 2) {
                    Scalar t = 2 - v;
                    return .25 * (t * t * t);
                    }
                    return 0 
                    },
                    P,v,radius,normalize);
            if(!normalize) {
                R /= ( M_PI * radius);
            }
            return R;
        }
    template <typename PointsType, typename VecType>
        auto  desbrun_spline_rbf(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v, typename PointsType::Scalar radius, bool normalize=false) {
            using Scalar = typename PointsType::Scalar;
            auto R = radial_basis_function(
                    [](Scalar v) -> Scalar { 
                    if(v < 2) {
                    Scalar t = 2 - v;
                    return t*t*t;
                    }
                    return 0 
                    },
                    P,v,radius,normalize);
            if(!normalize) {
                R *= 15;
                R /= ( M_PI * radius*radius*radius * 64);
            }
            return R;
        }
}
