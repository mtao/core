#pragma once
#include <iostream>
#include "mtao/types.hpp"
#include "mtao/eigen/shape_checks.hpp"
#include <Eigen/Sparse>

namespace mtao::geometry::interpolation {


    template <typename PointsType, typename VecType>
        auto  mean_value(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v) -> mtao::VectorX<typename PointsType::Scalar> {
            static_assert(std::is_same_v<typename PointsType::Scalar,typename VecType::Scalar>);
            eigen::row_check<2>(P);
            eigen::shape_check<2,1>(v);

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
            for(int i = 0; i < P.cols(); ++i) {
                int j = (i+1)%P.cols();
                R(j) = (tans(i) + tans(j)) / (P.col(j) - v).norm(); 
            }
            R /= R.sum();
            return R;
        }
    template <typename PointsType, typename VecType>
        auto  mean_value_gradient(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v) -> mtao::ColVectors<typename PointsType::Scalar, 2> {
            static_assert(std::is_same_v<typename PointsType::Scalar,typename VecType::Scalar>);
            eigen::row_check<2>(P);
            eigen::shape_check<2,1>(v);
            return {};
        }

    //https://dl.acm.org/citation.cfm?id=1281968
    //homogeneous exists to enable using barycentric coordinates in homogeneous space
    template <typename PointsType, typename VecType>
        auto  spherical_mean_value_unnormalized(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v) -> mtao::VectorX<typename PointsType::Scalar> {
            static_assert(std::is_same_v<typename PointsType::Scalar,typename VecType::Scalar>);
            eigen::row_check<3>(P);
            eigen::shape_check<3,1>(v);

            using Scalar = typename PointsType::Scalar;

            auto u = P.eval();//P normalized
            auto t = (u.colwise() - v).eval();
            t.colwise().normalize();
            u.colwise().normalize();
            auto n = u;//normals normalized
            mtao::VectorX<Scalar> alpha(P.cols());//angle between adjacent triangles -(\hat v_i - v) and \hat v_{i+1} - v
            mtao::VectorX<Scalar> theta(P.cols());//angle between neighboring vertices
            mtao::VectorX<Scalar> cot_theta(P.cols());//cot(theta_j)
            mtao::VectorX<Scalar> sin_theta(P.cols());//sin(theta_j)
            mtao::VectorX<Scalar> tan_alpha_o_2(P.cols());//tan(alpha_j / 2)



            for(int j = 0; j < P.cols(); ++j) {
                int jm1 = (j - 1 + P.cols())%P.cols();
                int jp1 = (j + 1)%P.cols();
                auto v1 = -t.col(j);
                auto v2 = t.col(jp1);

                n.col(j) = t.col(j).cross(t.col(jp1));
                sin_theta(j) = n.col(j).norm();
                theta(j) = std::asin( n.col(j).norm() / 2);
                Scalar tc = t.col(j).dot(t.col(jp1));
                if(std::abs(tc) > 1e-5) {
                    cot_theta(j) = sin_theta(j) / tc;
                } else {
                    cot_theta(j) = std::numeric_limits<Scalar>::max()/(4 * P.cols());//Just set to a very large number
                }

                Scalar c = v1.dot(v2);
                alpha(j) = std::acos(c);
                Scalar s = (v1.cross(v2)).norm();
                tan_alpha_o_2(j) = std::sqrt((1-c)/(1+c));
                n.col(j).normalize();
            }



            //std::cout << "=============" << std::endl;
            //std::cout << theta.transpose() << std::endl;
            //std::cout << "**" << std::endl;
            //std::cout << n << std::endl;
            //std::cout << "==" << std::endl;
            mtao::Vector<Scalar,3> v_F = .5 * (n * theta);
            //std::cout << v_F.transpose() << std::endl;
            Scalar v_Fn = v_F.norm();


            mtao::VectorX<Scalar> R(P.cols());
            R.setZero();
            Scalar denom = 0;

            //std::cout << "vvv" << std::endl;
            //std::cout << "VFN: " << v_F << std::endl;
            //std::cout << n << std::endl;
            //std::cout << "Theta: " << theta.transpose() << std::endl;
            //std::cout << "alpha:" << alpha.transpose() << std::endl;
            //std::cout << "tanalpha:" << tan_alpha_o_2.transpose() << std::endl;
            //std::cout << "cot_theta:" << cot_theta.transpose() << std::endl;
            //std::cout << "^^^" << std::endl;
            for(int i = 0; i < P.cols(); ++i) {
                int im1 = (i-1+P.cols())%P.cols();
                denom += cot_theta(i) * (tan_alpha_o_2(im1) + tan_alpha_o_2(i));
            }
            for(int i = 0; i < P.cols(); ++i) {
                int ip1 = (i+1)%P.cols();
                int im1 = (i-1+P.cols())%P.cols();
                //std::cout << ((v_Fn)/(P.col(i) - v).norm()) << " || "
                //    << ((tan_alpha_o_2(im1) + tan_alpha_o_2(i))/sin_theta(i)) << std::endl;;
                R(i) = ((v_Fn)/(P.col(i) - v).norm()) * 
                    ((tan_alpha_o_2(im1) + tan_alpha_o_2(i))/sin_theta(i)) / denom;
            }
            return R;
        }
    template <typename PointsType, typename VecType>
        auto  spherical_mean_value(const Eigen::MatrixBase<PointsType>& P, const Eigen::MatrixBase<VecType>& v) -> mtao::VectorX<typename PointsType::Scalar> {
            auto R = spherical_mean_value_unnormalized(P,v);
            return R /= R.sum();
        }

    template <typename PointsType, typename TriangleType, typename VecType>
        auto  spherical_mean_value(const Eigen::MatrixBase<PointsType>& V, const Eigen::MatrixBase<TriangleType>& F, const Eigen::MatrixBase<VecType>& v) -> Eigen::SparseMatrix<typename PointsType::Scalar> {
            eigen::row_check<3>(V);
            eigen::row_check<3>(F);
            using Scalar = typename PointsType::Scalar;
            Eigen::SparseMatrix<Scalar> A(1,V.cols());
            std::vector<Eigen::Triplet<Scalar>> trips;
            for(int i = 0; i < F.cols(); ++i) {
                mtao::Matrix<Scalar,PointsType::RowsAtCompileTime,TriangleType::RowsAtCompileTime> M(V.rows(),F.rows());
                auto f = F.col(i);
                for(int j = 0; j < F.rows(); ++j) {
                    M.col(j) = V.col(f(j));
                }
                auto c = spherical_mean_value_unnormalized(M,v);
                //std::cout << "just the poly: " << std::endl;
                //std::cout << M << std::endl;
                //std::cout << "result data: " << c.transpose() << std::endl;
                for(int j = 0; j < F.rows(); ++j) {
                    trips.emplace_back(0,f(j),c(j));
                }
            }
            A.setFromTriplets(trips.begin(),trips.end());
            return A / A.sum();
        }
}
