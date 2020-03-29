#pragma once
#include "mtao/eigen/shape_checks.hpp"
#include "mtao/types.hpp"

namespace mtao::geometry::mesh {
// allows for edge sorting, useful when edges are mapped into
// by defualt returns V(e(1)) - V(e(0))
template <typename VDerived, typename EDerived>
auto edge_tangents(const Eigen::MatrixBase<VDerived>& V,
                   const Eigen::MatrixBase<EDerived>& E,
                   bool sort_edges = false) {
    eigen::row_check<2>(E);
    using Scalar = typename VDerived::Scalar;
    using TType = ColVectors<Scalar, VDerived::RowsAtCompileTime>;
    TType T = TType::Zero(V.rows(), E.cols());
    for (int i = 0; i < E.cols(); ++i) {
        auto e = E.col(i);
        auto a = V.col(e(0));
        auto b = V.col(e(1));
        if (sort_edges && e(0) > e(1)) {
            T.col(i) = a - b;
        } else {
            T.col(i) = b - a;
        }
    }
    return T;
}
}  // namespace mtao::geometry::mesh
