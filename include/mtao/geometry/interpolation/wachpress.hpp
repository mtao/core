#pragma once
#include "mtao/types.hpp"
#include "mtao/eigen/shape_checks.hpp"
#include "mtao/eigen/stack.hpp"

namespace mtao::geometry::interpolation {


    template <typename PointsType, typename VecType>
        auto  wachpress_areas(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v) -> mtao::VectorX<typename PointsType::Scalar> {
            static_assert(std::is_same_v<typename PointsType::Scalar,typename VecType::Scalar>);
            eigen::row_check_with_assert<2>(P);
            eigen::shape_check_with_assert<2,1>(v);

            using Scalar = typename PointsType::Scalar;
            mtao::VectorX<Scalar> wedge_vols(P.cols());
            mtao::VectorX<Scalar> pie_vols(P.cols());
            //mtao::SquareMatrix<Scalar,2> A;
            //mtao::SquareMatrix<Scalar,2> B;
            mtao::SquareMatrix<Scalar,3> A;
            A.row(0).setConstant(1);
            for(int i = 0; i < P.cols(); ++i) {
                auto a = P.col(i); 
                auto b = P.col((i+1)%P.cols()); 
                auto c = P.col((i+2)%P.cols()); 
                /*
                A.col(0) = a - v;
                A.col(1) = b - v;
                B.col(0) = a - c;
                B.col(1) = b - c;
                pie_vols(i) = A.determinant() ;
                wedge_vols((i+1)%P.cols()) = B.determinant(); 
                */
                A.template block<2,1>(1,0) = a;
                A.template block<2,1>(1,1) = b;
                A.template block<2,1>(1,2) = v;
                pie_vols(i) = .5 * A.determinant() ;
                A.template block<2,1>(1,2) = c;
                wedge_vols((i+1)%P.cols()) = .5 * A.determinant(); 
            }
            mtao::VectorX<Scalar> R(P.cols());
            for(int j = 0; j < P.cols(); ++j) {
                int i = (j+1)%P.cols();
                R(i) = wedge_vols(i) / (pie_vols(j)*pie_vols(i));
            }
            R /= R.sum();
            return R;
        }
    template <typename PointsType, typename VecType>
        auto  wachpress_perpendicular(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v) -> mtao::VectorX<typename PointsType::Scalar> {
            static_assert(std::is_same_v<typename PointsType::Scalar,typename VecType::Scalar>);
            eigen::col_check_with_assert<1>(v);

            using Scalar = typename PointsType::Scalar;
            mtao::VectorX<Scalar> H(P.cols());
            mtao::ColVectors<Scalar,2> N(2,P.cols());
            for(int i = 0; i < P.cols(); ++i) {
                auto a = P.col(i); 
                auto b = P.col((i+1)%P.cols()); 
                auto t = b - a;
                auto n = N.col(i) ;
                n.x() = -t.y();
                n.y() = t.x();
                H(i) = (a - v).dot(n);

            }
            mtao::VectorX<Scalar> R(P.cols());
            for(int j = 0; j < P.cols(); ++j) {
                int i = (j+1)%P.cols();
                Scalar nn = mtao::eigen::hstack(N.col(j),N.col(i)).determinant();
                R(i) = nn / (H(i)*H(j));
            }
            R /= R.sum();
            return R;
        }
    template <typename PointsType, typename VecType>
        auto  wachpress(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v, bool use_perpendicular = true) -> mtao::VectorX<typename PointsType::Scalar> {
            if(use_perpendicular) {
                return wachpress_perpendicular(P,v);
            } else {
                return wachpress_areas(P,v);
            }
        }
    template <typename PointsType, typename VecType>
        auto  wachpress_gradient(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v) -> mtao::ColVectors<typename PointsType::Scalar,2> {
            static_assert(std::is_same_v<typename PointsType::Scalar,typename VecType::Scalar>);
            eigen::row_check_with_assert<2>(P);
            eigen::shape_check_with_assert<2,1>(v);

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
