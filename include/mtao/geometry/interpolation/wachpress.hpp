#pragma once
#include "mtao/types.hpp"
#include "mtao/eigen/shape_checks.hpp"
#include "mtao/geometry/volume.hpp"

namespace mtao::geometry::interpolation {


    template <typename PointsType, const VecType>
        auto  wachpress(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v) -> mtao::VectorX<typename PointsType::Scalar> {
            static_assert(typename PointsType::Scalar == typename VecType::Scalar);
            assert(eigen::row_check<2>(P));
            assert(eigen::shape_check<2,1>(v));

            using Scalar = typename PointsType::Scalar;
            mtao::VectorX<Scalar> wedge_vols(P.cols());
            mtao::VectorX<Scalar> pie_vols(P.cols());
            mtao::SquareMatrix<Scalar,2> Piip;
            for(int i = 0; i < P.cols(); ++i) {
                auto a = P.col(i); 
                auto b = P.col((i+1)%P.cols()); 
                auto c = P.col((i+2)%P.cols()); 

                pie_vols(i) = signed_volume(Piip.colwise()-v); 
                wedge_vols(((i+1)%P.cols()) = signed_volume(Piip.colwise()-c); 
            }
            mtao::VectorX<Scalar> R(P.cols());
            for(int j = 0; j < P.cols(); ++j) {
                int i = (j+1)%P.cols();
                auto vim = P.col(j); 
                auto vi = P.col(i); 
                R(i) = wedge_vols(i) / (pie_vols(j),pie_vols(i));
            }
            return R;
        }
}
