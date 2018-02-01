#pragma once
#include "mtao/types.h"
#include "mtao/geometry/volume.h"
#include <iostream>
namespace mtao { namespace geometry {
    template <typename T, int C>
    auto barycentric_2d_convex(const ColVectors<T,2>& P, const mtao::Vector<int,C>& indices, const mtao::Vector<T,2>& p) -> mtao::Vector<T,C> {

        int cell_size = indices.rows();
        mtao::Vector<T,C> weights(cell_size);
        //T weight_sum = 0;
        constexpr static T epsilon = 1e-3;
        using Vec = mtao::Vector<T,2>;

        auto boundary_dist = [&](auto&& a, auto&& b) -> T {
            auto pa = p-a;
            auto ba = (b-a).normalized().eval();
            return (pa - ba.dot(pa) * ba).norm();
        };
        auto cotangent = [](const Vec& a, const Vec& b, const Vec& c) -> T {
            auto&& ba = (a-b).normalized();
            auto&& bc = (c-b).normalized();
            auto&& dt = bc.dot(ba) ;
            return dt/std::sqrt(1-dt*dt);
        };


        for(int i = 0; i < cell_size; ++i) {
            int curr = indices(i);
            int ip1 = (i+1)%cell_size;
            int next = indices[ip1];
            auto&& q = P.col(curr);
            auto&& qn = P.col(next);
            auto err = epsilon * (qn-q).norm();
            if((p-q).norm() < err) {//Sits on this vertex
                return mtao::Vector<T,C>::Unit(cell_size,i);
            } else if(boundary_dist(q,qn) <= err) {//sits on an edge
                weights.setZero();
                T dn = (p-qn).norm();
                T d = (p-q).norm();
                weights(i) = dn/(d+dn);
                weights(ip1) = d/(d+dn);
                return weights;
            }
        }
        for(int i = 0; i < indices.size(); ++i) {
            int prev = indices((i+cell_size-1)%cell_size);
            int curr = indices(i);
            int next = indices((i+1)%indices.size());
            auto&& qp = P.col(prev);
            auto&& q = P.col(curr);
            auto&& qn = P.col(next);
            weights(i) = (cotangent(p,q,qp) + cotangent(p,q,qn)) / (p-q).squaredNorm();
        }
        return weights / weights.sum();

    }
    template <typename T, int D, int C>
        auto barycentric_simplicial(const Matrix<T,D,C>& P, const mtao::Vector<T,D>& p) -> mtao::Vector<T,C> {
            std::cout << std::endl;

            mtao::Vector<T,C> ret;

            auto V = volume_unsigned(P);
            Matrix<T,D,C> SC = P;
            for(int i = 0; i < C; ++i) {
                SC.col(i) = p;
                ret(i) = volume_unsigned(SC);
                SC.col(i) = P.col(i);
            }
            return ret / V;




            /*
            for(int i = 0; i < C; ++i) {
                auto n = N.col(i);
                int i1 = (i+1)%C;
                auto o = P.col(i1);
                auto d = P.col(i);
                n = o -  d;
                std::cout <<  i << "->" << i1 << " = " << n.transpose() << std::endl;
                for(int j = (i1+1)%C; j!=i; j = (j+1)%C) {
                    mtao::Vector<T,D> l = P.col(j) - o;
                    l.normalize();
                    std::cout << "Projecting with: " << j  << ": " << l.transpose()<< std::endl;
                    n = n - l.dot(n) * l;
                    std::cout << "new N: " << n.transpose() << std::endl;
                }
                auto nn = n.norm();
                n = (n / nn ) / nn;
                std::cout << "Resulting N: " << n.transpose() << std::endl;
            }


            for(int i = 0; i < C; ++i) {
                ret(i) = 1 - N.col(i).dot(p - P.col(i));
            }

            return ret;
            */
        }

}}
