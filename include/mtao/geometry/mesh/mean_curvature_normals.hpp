#pragma once
#include "mtao/geometry/mesh/laplacian.hpp"
#include "mtao/geometry/circumcenter.h"

namespace mtao {
namespace geometry {
    namespace mesh {

        //Mean curvature normals
        //TODO: this does not select the right orientation

        template<typename VDerived, typename FDerived>
        auto mean_curvature_normals(const Eigen::MatrixBase<VDerived> &V, const Eigen::MatrixBase<FDerived> &F, bool normalized = true) {

            auto R = (V * strong_cot_laplacian(V, F).transpose()).eval();
            if (normalized) {
                R.colwise().normalize();
                auto C = circumcenters(V, F);
                for (int i = 0; i < R.cols(); ++i) {
                    auto r = R.col(i);
                    auto v = V.col(i);
                    auto c = C.col(i);
                    if ((v - c).dot(r) < 0) {
                        r *= -1;
                    }
                }

            } else {
                //Normals might face the wrong way
            }
            return R;
        }
    }// namespace mesh
}// namespace geometry
}// namespace mtao
