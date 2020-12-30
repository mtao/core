#pragma once

#include "mtao/types.hpp"
#include "mtao/geometry/volume.h"
namespace mtao {
namespace geometry {
    namespace mesh {
        template<typename VertexDerived, typename SimplexDerived>
        auto opposite_volumes(const Eigen::MatrixBase<VertexDerived> &V, const Eigen::MatrixBase<SimplexDerived> &S) {

            using Scalar = typename VertexDerived::Scalar;
            mtao::ColVectors<Scalar, SimplexDerived::RowsAtCompileTime> L(S.rows(), S.cols());

            const int simplex_size = S.rows();
            const int sm1 = simplex_size - 1;
            for (int i = 0; i < S.cols(); ++i) {
                auto s = S.col(i);
                auto l = L.col(i);
                mtao::ColVectors<Scalar, VertexDerived::RowsAtCompileTime> VV(V.rows(), sm1);
                for (int j = 0; j < sm1; ++j) {
                    VV.col(j) = V.col(s(j));
                }
                for (int j = 0; j < simplex_size; ++j) {
                    VV.col(j % sm1) = V.col(s((j + sm1) % simplex_size));
                    l(j) = mtao::geometry::volume_unsigned(VV);
                }
            }
            return L;
        }
    }// namespace mesh
}// namespace geometry
}// namespace mtao
