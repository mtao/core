#pragma once
#include "mtao/types.hpp"
#include "mtao/eigen/shape_checks.hpp"

namespace mtao::geometry::interpolation {


    template <typename PointsType, typename VecType>
        auto  wachpress(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v) -> mtao::VectorX<typename PointsType::Scalar> {
            static_assert(std::is_same_v<typename PointsType::Scalar,typename VecType::Scalar>);
            eigen::row_check<2>(P);
            eigen::shape_check<2,1>(v);

            using Scalar = typename PointsType::Scalar;
            mtao::VectorX<Scalar> wedge_vols(P.cols());
            mtao::VectorX<Scalar> pie_vols(P.cols());
            mtao::SquareMatrix<Scalar,2> A;
            mtao::SquareMatrix<Scalar,2> B;
            for(int i = 0; i < P.cols(); ++i) {
                auto a = P.col(i); 
                auto b = P.col((i+1)%P.cols()); 
                auto c = P.col((i+2)%P.cols()); 
                A.col(0) = a - v;
                A.col(1) = b - v;
                B.col(0) = a - c;
                B.col(1) = b - c;
                pie_vols(i) = A.determinant() ;
                wedge_vols((i+1)%P.cols()) = B.determinant(); 
            }
            mtao::VectorX<Scalar> R(P.cols());
            for(int j = 0; j < P.cols(); ++j) {
                int i = (j+1)%P.cols();
                R(i) = wedge_vols(i) / (pie_vols(j)*pie_vols(i));
            }
            R /= R.sum();
            return R;
        }
}
