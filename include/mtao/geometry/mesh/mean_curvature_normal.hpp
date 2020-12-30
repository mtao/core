#pragma once
#include "mtao/geometry/mesh/laplacian.hpp"

namespace mtao {
namespace geometry {
    namespace mesh {

        //Area weighted normals

        template<typename VDerived, typename FDerived>
        auto mean_curvature_normals(const Eigen::MatrixBase<VDerived> &V, const Eigen::MatrixBase<FDerived> &F, bool normalized = true) {

            auto R = (V * strong_cot_laplacian(V, F).transpose()).eval();
            if (normalized) {
                R.colwise().normalize();
            }
            return R;
        }
    }// namespace mesh
}// namespace geometry
}// namespace mtao
