#pragma once
#include "mtao/types.hpp"

namespace mtao { namespace geometry { namespace mesh {

    template <typename VDerived, typename FDerived>
        auto face_normals(const Eigen::MatrixBase<VDerived>& V, const Eigen::MatrixBase<FDerived>& F) {
            constexpr static int CRows = VDerived::RowsAtCompileTime;
            auto N = VDerived::Zero(V.rows(),F.cols()).eval();
            using T = typename VDerived::Scalar;
            if constexpr(CRows == 2) {
            for(int i = 0; i < F.cols(); ++i) {
                auto e = F.col(i);
                auto a = V.col(e(0));
                auto b = V.col(e(1));

                mtao::Vector<T,2> ba = b-a;
                N.col(i) = mtao::Vector<T,2>(-ba.y(),ba.x());
            }
            } else if constexpr(CRows == 3) {

            for(int i = 0; i < F.cols(); ++i) {
                auto f = F.col(i);
                auto a = V.col(f(0));
                auto b = V.col(f(1));
                auto c = V.col(f(2));

                mtao::Vector<T,3> ba = b-a;
                mtao::Vector<T,3> ca = c-a;
                N.col(i) = ba.cross(ca);
            }
            }
            N.colwise().normalize();

            return N;
        }
    //Area weighted normals


}}}
