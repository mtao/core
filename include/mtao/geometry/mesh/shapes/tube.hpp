#pragma once
#include "mtao/geometry/bishop_frame.hpp"
#include "mtao/geometry/mesh/shapes/disc.hpp"


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
                    auto r = RV(Eigen::all,Eigen::seqN(subdivisions*i,subdivisions));
                    r = radius * mtao::eigen::hstack(u,v) * CS;
                    r.colwise() += p;

                    if(i == P.cols()-1) continue;
                    auto f = RF(Eigen::all,Eigen::seqN(2*subdivisions*i,2*subdivisions));
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
                        RF = mtao::eigen::hstack(RF,CF,CF({0,2,1},Eigen::all).array()+(P.cols()-1)*subdivisions);
                    } else {
                        if(front) {
                            RF = mtao::eigen::hstack(RF,CF);
                        } else {
                            RF = mtao::eigen::hstack(RF,CF({0,2,1},Eigen::all).array()+(P.cols()-1)*subdivisions);
                        }
                    }
                }

                return {RV,RF};

            }
    }
}
