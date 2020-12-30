#pragma once
#include "mtao/types.hpp"

namespace mtao::geometry::mesh {

template<typename SimplexDerived>
auto vertex_valence(const Eigen::MatrixBase<SimplexDerived> &S) -> mtao::VecXi {
    if (S.size() == 0) {
        return {};
    }
    mtao::VecXi C = mtao::VecXi::Zero(S.maxCoeff() + 1);
    for (int i = 0; i < S.size(); ++i) {
        C(S(i))
        ++;
    }
    return C;
}
}// namespace mtao::geometry::mesh
