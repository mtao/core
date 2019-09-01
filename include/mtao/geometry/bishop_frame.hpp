#pragma once
#include <mtao/types.hpp>


namespace mtao::geometry {


    template <typename S>
        std::array<mtao::ColVectors<S,3>,2> bishop_frame_from_tangents(const mtao::ColVectors<S,3>& T) {
            using CVecs = typename mtao::ColVectors<S,3>;
            using Vec = mtao::Vector<S,3>;

            Vec t = T.col(0);
            Vec u = t.cross(Vec::Unit(0));
            if(u.norm() < 1e-5) {
                u = t.cross(Vec::Unit(1));
            }

            CVecs U(3,T.cols());
            U.col(0) = u.normalized();
            for(int i = 1; i < U.cols(); ++i) {
                Vec p = T.col(i-1).cross(T.col(i));
                Vec axis = p.normalized();
                S s = p.norm();
                S c = T.col(i-1).dot(T.col(i));

                S theta = std::atan2(s,c);

                auto P = Eigen::AngleAxis<S>(theta,axis);
                Vec tu =  P * T.col(i-1);
                if((tu-T.col(i)).norm() > 1e-2) {
                    std::cout << "FRAME COMPUTATION FAILURE!" << std::endl;
                }
                t = P * t;
                u = P * u;
                U.col(i) = u.normalized();
            }
            return {{T,U}};
        }
    template <typename S>
        std::array<mtao::ColVectors<S,3>,2> bishop_frame(const mtao::ColVectors<S,3>& V) {
            using CVecs = typename mtao::ColVectors<S,3>;

            bool use_first_order = true;
            CVecs T;
            if(!use_first_order) {
                T.resizeLike(V);
                T.rightCols(V.cols()-1) = V.rightCols(V.cols()-1) - V.leftCols(V.cols()-1);
                T.col(0) = T.col(1);
                T.colwise().normalize();
            } else {
                T.resizeLike(V);
                T.col(0) = V.col(1) - V.col(0);
                for(int i = 1; i < V.cols() - 1; ++i) {
                    T.col(i) = V.col(i+1) - V.col(i-1);
                }
                T.col(V.cols()-1) = V.col(V.cols()-1) - V.col(V.cols()-2);
                T.colwise().normalize();
                T.noalias() = T.array().isFinite().select(T,0);
            }
            return bishop_frame_from_tangents(T);
        }


}
