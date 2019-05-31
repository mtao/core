#pragma once
#include "mtao/types.hpp"

namespace mtao { namespace geometry { namespace mesh {

    //Area weighted normals

    template <typename VDerived, typename FDerived>
        auto vertex_normals(const Eigen::MatrixBase<VDerived>& V, const Eigen::MatrixBase<FDerived>& F, bool normalized = true) {
            constexpr static int CRows = VDerived::RowsAtCompileTime;
            auto N = VDerived::Zero(V.rows(),V.cols()).eval();
            using T = typename VDerived::Scalar;
            if constexpr(CRows == 2 || CRows == Eigen::Dynamic) {
            if (F.rows()== 2) {
                for(int i = 0; i < F.cols(); ++i) {
                    auto e = F.col(i);
                    auto a = V.col(e(0));
                    auto b = V.col(e(1));

                    mtao::Vector<T,2> ba = b-a;
                    mtao::Vector<T,2> n(-ba.y(),ba.x());
                    for(int j = 0; j < 2; ++j) {
                        N.col(e(j)) += n;
                    }
                }
            } 
            }
            if constexpr(CRows == 3 || CRows == Eigen::Dynamic) {
            if (F.rows() == 3) {

                for(int i = 0; i < F.cols(); ++i) {
                    auto f = F.col(i);
                    for(int j = 0; j < 3; ++j) {
                        int ai = f((j + 0)%3);
                        int bi = f((j + 1)%3);
                        int ci = f((j + 2)%3);
                        auto a = V.col(ai);
                        auto b = V.col(bi);
                        auto c = V.col(ci);

                        mtao::Vector<T,3> ba = b-a;
                        mtao::Vector<T,3> ca = c-a;
                        mtao::Vector<T,3> n = ba.cross(ca);
                        N.col(bi) += n;
                        N.col(ci) += n;

                    }
                }
            }
            }
            if(normalized) {
                N.colwise().normalize();
            }

            return N;
        }


}}}
