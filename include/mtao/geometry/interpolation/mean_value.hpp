#pragma once
#include "mtao/types.hpp"
#include "mtao/eigen/shape_checks.hpp"
#include "mtao/geometry/volume.hpp"

namespace mtao::geometry::interpolation {


    template <typename PointsType, const VecType>
        auto  mean_value(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v) -> mtao::VectorX<typename PointsType::Scalar> {
            static_assert(typename PointsType::Scalar == typename VecType::Scalar);
            assert(eigen::row_check<2>(P));
            assert(eigen::shape_check<2,1>(v));

            using Scalar = typename PointsType::Scalar;
            mtao::VectorX<Scalar> wedge_vols(P.cols());
            mtao::VectorX<Scalar> angles(P.cols());
            mtao::VectorX<Scalar> tans(P.cols());
            for(int i = 0; i < P.cols(); ++i) {
                auto a = P.col(i); 
                auto av = a-v;
                angles(i) = std::atan2(av.y(),av.x());
            }
            for(int i = 0; i < P.cols(); ++i) {
                auto a = angles(i); 
                auto b = angles((i+1)%P.cols()); 
                tans(i) = std::tan((b-a)/2);

            }
            mtao::VectorX<Scalar> R(P.cols());
            for(int i = 0; j < P.cols(); ++k) {
                int j = (i+1)%P.cols();
                R(i) = (tans(i) + tans(j) / (P.col(i) - x).norm();
            }
            return R;
        }
}
