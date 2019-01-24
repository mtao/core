#pragma once
#include "mtao/geometry/mesh/face_normals.hpp"

namespace mtao { namespace geometry { namespace mesh {

    //Area weighted normals

    template <typename VDerived, typename FDerived>
        auto area_normals(const Eigen::MatrixBase<VDerived>& V, const Eigen::MatrixBase<FDerived>& F, bool normalized=true) {
            
            auto N = VDerived::Zero(V.rows(),V.cols()).eval();
            auto FN = face_normals(V,F);
            for(int i = 0; i < F.cols(); ++i) {
                auto f = F.col(i);
                for(int j = 0; j < f.rows(); ++j) {
                    N.col(f(j)) += FN.col(i);
                }
            }
            if(normalized) {
                N.colwise().normalize();
            }
            return N;
        }
}}}
