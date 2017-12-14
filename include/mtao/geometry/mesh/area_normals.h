#pragma once
#include "mtao/types.h"

namespace mtao { namespace geometry { namespace mesh {

    //Area weighted normals

    template <typename T>
        mtao::ColVectors<T,3> area_normals(const ColVectors<T,3>& V, const ColVectors<int,3>& F) {
            
            auto N = mtao::ColVectors<T,3>::Zero(3,V.cols()).eval();
            for(int i = 0; i < F.cols(); ++i) {
                auto f = F.col(i);
                auto a = V.col(f(0));
                auto b = V.col(f(1));
                auto c = V.col(f(2));

                mtao::Vector<T,3> ba = b-a;
                mtao::Vector<T,3> ca = c-a;
                mtao::Vector<T,3> n = ba.cross(ca);
                for(int j = 0; j < 3; ++j) {
                    N.col(f(j)) += n;
                }
            }
            N.colwise().normalize();

            return N;
        }

    template <typename T>
        mtao::ColVectors<T,2> area_normals(const ColVectors<T,2>& V, const ColVectors<int,2>& E) {
            
            auto N = mtao::ColVectors<T,2>::Zero(2,V.cols()).eval();
            for(int i = 0; i < E.cols(); ++i) {
                auto e = E.col(i);
                auto a = V.col(e(0));
                auto b = V.col(e(1));

                mtao::Vector<T,2> ba = b-a;
                mtao::Vector<T,2> n(-ba.y(),ba.x());
                for(int j = 0; j < 2; ++j) {
                    N.col(e(j)) += n;
                }
            }
            N.colwise().normalize();

            return N;
        }

}}}
