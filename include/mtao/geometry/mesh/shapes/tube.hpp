#pragma once
#include "mtao/geometry/bishop_frame.hpp"
#include "mtao/geometry/mesh/shapes/disc.hpp"
#include <iostream>


namespace mtao::geometry::mesh::shapes {
    namespace internal {
        template <typename T>
        std::tuple<mtao::ColVectors<T,3>,mtao::ColVecs3i> tube(const mtao::ColVectors<T,3>& P, double radius, int subdivisions, bool back, bool front);
    }

    //only back half capped
    template <typename T>
        std::tuple<mtao::ColVectors<T,3>,mtao::ColVecs3i> tube(const mtao::ColVectors<T,3>& P, double radius, int subdivisions = 5, bool back=false,bool front=false) {
            return internal::tube(P,radius,subdivisions,back,front);
        }
    template <typename T>
        std::tuple<mtao::ColVectors<T,3>,mtao::ColVecs3i> capped_tube(const mtao::ColVectors<T,3>& P, double radius, int subdivisions = 5) {
            return internal::tube(P,radius,subdivisions,true,true);
        }
    namespace internal {
        template <typename T>
            std::tuple<mtao::ColVectors<T,3>,mtao::ColVecs3i> tube(const mtao::ColVectors<T,3>& P, double radius, int subdivisions, bool back, bool front) {
                using CVec = mtao::ColVectors<T,3>;
                using VecX = mtao::VectorX<T>;

                VecX theta = VecX::LinSpaced(subdivisions+1,0,2*M_PI).head(subdivisions);


                mtao::ColVectors<T,2> CS = mtao::eigen::hstack(theta.array().cos(),theta.array().sin()).transpose();

                CVec RV(3,subdivisions * P.cols());

                auto [TT,U] = bishop_frame(P);
                CVec V(3,P.cols());
                mtao::ColVecs3i RF(3,2*(P.cols()-1)*subdivisions);
                for(int i = 0; i < V.cols(); ++i) {
                    V.col(i) = TT.col(i).cross(U.col(i));
                }


                for(int i = 0; i < P.cols(); ++i) {
                    auto p = P.col(i);
                    auto u = U.col(i);
                    auto v = V.col(i);
                    int voff = subdivisions*i;
                    auto r = RV.block(0,subdivisions*i,RV.rows(),subdivisions);
                    //auto r = RV(Eigen::all,Eigen::seqN(subdivisions*i,subdivisions));
                    if(false && i > 0 && i < P.cols()-1) {

                        mtao::Vector3<T> tm = P.col(i-1) - P.col(i);
                        mtao::Vector3<T> t = P.col(i+1)-P.col(i);
                        tm.normalize();
                        t.normalize();
                        double shift = 1.0/std::sqrt(1-std::abs(tm.normalized().dot(t.normalized())));
                        mtao::Vector3<T> axis = tm.cross(t).normalized();
                        mtao::SquareMatrix<T,2> A;
                        A(0,0) = u.dot(axis);
                        A(0,1) = v.dot(axis);
                        A.row(0).normalize();
                        A(1,1) = A(0,0);
                        A(1,0) = -A(0,1);
                        A = A.transpose() * mtao::Vector2<T>(T(1),T(shift)).asDiagonal() * A.transpose().inverse();
                        r = radius * mtao::eigen::hstack(u,v) * A * CS;
                    } else {
                        r = radius * mtao::eigen::hstack(u,v) * CS;
                    }
                    r.colwise() += p;

                    if(i == P.cols()-1) continue;
                    //auto f = RF(Eigen::all,Eigen::seqN(2*subdivisions*i,2*subdivisions));
                    auto f = RF.block(0,2*subdivisions*i,RF.rows(),2*subdivisions);
                    for(int j = 0; j < subdivisions; ++j) {
                        int u = voff+j;
                        int v = voff+(j+1)%subdivisions;
                        int s = voff+subdivisions+j;
                        int t = voff+subdivisions+(j+1)%subdivisions;
                        f.col(2*j) << u,v,s;
                        f.col(2*j+1) << s,v,t;


                    }


                }



                if(front || back) {
                    auto CF =internal::disc_faces(subdivisions);
                    if(front & back) {
                    mtao::ColVecs3i CSwiz(CF.rows(),CF.cols());
                    CSwiz.row(0) = CF.row(0);
                    CSwiz.row(1) = CF.row(2);
                    CSwiz.row(2) = CF.row(1);
                        //RF = mtao::eigen::hstack(RF,CF,CF({0,2,1},Eigen::all).array()+(P.cols()-1)*subdivisions);
                        RF = mtao::eigen::hstack(RF,CF,CSwiz.array()+(P.cols()-1)*subdivisions);
                    } else {
                        if(front) {
                            RF = mtao::eigen::hstack(RF,CF);
                        } else {
                    mtao::ColVecs3i CSwiz(CF.rows(),CF.cols());
                    CSwiz.row(0) = CF.row(0);
                    CSwiz.row(1) = CF.row(2);
                    CSwiz.row(2) = CF.row(1);
                            //RF = mtao::eigen::hstack(RF,CF({0,2,1},Eigen::all).array()+(P.cols()-1)*subdivisions);
                            RF = mtao::eigen::hstack(RF,CSwiz.array()+(P.cols()-1)*subdivisions);
                        }
                    }
                }

                return {RV,RF};

            }
    }
}
