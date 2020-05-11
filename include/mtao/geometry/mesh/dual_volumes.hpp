#pragma once
#include "mtao/geometry/mesh/vertex_valence.hpp"
#include "mtao/geometry/volume.hpp"

namespace mtao::geometry::mesh {

    // circumcentral vertex volumes
template <typename VertexDerived, typename SimplexDerived>
auto dual_volumes(const Eigen::MatrixBase<VertexDerived>& V,
                  const Eigen::MatrixBase<SimplexDerived>& S) {
    auto vol = geometry::volumes(V, S);

    using Vec = mtao::VectorX<typename VertexDerived::Scalar>;
    Vec dual_vol = Vec::Zero(V.cols());

    for (int i = 0; i < S.cols(); ++i) {
        const auto& v = vol(i);
        auto s = S.col(i);
        for (int j = 0; j < S.rows(); ++j) {
            dual_vol(s(j)) += v;
        }
    }
    dual_vol /= static_cast<typename VertexDerived::Scalar>(S.rows());
    return dual_vol;
}
}  // namespace mtao::geometry::mesh
