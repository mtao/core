
#pragma once
#include "compactify.hpp"

namespace mtao::geometry::mesh {

template<typename VDerived, typename CDerived>
auto separate_elements(const Eigen::MatrixBase<VDerived> &V, const std::map<int, CDerived> &Cs) {

    constexpr static int VRows = VDerived::RowsAtCompileTime;
    using Scalar = typename VDerived::Scalar;
    using RetVerts = mtao::ColVectors<Scalar, VRows>;

    constexpr static int CRows = CDerived::RowsAtCompileTime;
    using Index = typename CDerived::Scalar;
    using RetCells = mtao::ColVectors<Index, CRows>;

    std::map<int, std::tuple<RetVerts, RetCells>> ret;

    for (auto &&[idx, C] : Cs) {
        ret[idx] = compactify(V, C);
    }
    return ret;
}
template<typename VDerived, typename CDerived>
auto separate_elements(const Eigen::MatrixBase<VDerived> &V, const std::vector<CDerived> &Cs) {

    constexpr static int VRows = VDerived::RowsAtCompileTime;
    using Scalar = typename VDerived::Scalar;
    using RetVerts = mtao::ColVectors<Scalar, VRows>;

    constexpr static int CRows = CDerived::RowsAtCompileTime;
    using Index = typename CDerived::Scalar;
    using RetCells = mtao::ColVectors<Index, CRows>;

    std::vector<std::tuple<RetVerts, RetCells>> ret(Cs.size());

    std::transform(Cs.begin(), Cs.end(), ret.begin(), [&](auto &&F) {
        return separate_elements(V, F);
    });
    return ret;
}

template<typename VDerived, typename CDerived>
auto unify(const std::map<int, std::tuple<VDerived, CDerived>> &VM) {
    constexpr static int VRows = VDerived::RowsAtCompileTime;
    using Scalar = typename VDerived::Scalar;
    using RetVerts = mtao::ColVectors<Scalar, VRows>;

    constexpr static int CRows = CDerived::RowsAtCompileTime;
    using Index = typename CDerived::Scalar;
    using RetCells = mtao::ColVectors<Index, CRows>;
    int vcols = 0;
    int ccols = 0;
    int vrows = 0;
    int crows = 0;

    for (auto &&[i, VC] : VM) {
        auto [V, C] = VC;
        vcols += V.cols();
        ccols += C.cols();
        vrows = std::max<int>(vrows, V.rows());
        crows = std::max<int>(crows, C.rows());
    }
    RetVerts mV(vrows, vcols);
    RetCells mC(crows, ccols);
    std::vector<int> blocks;
    vcols = 0;
    ccols = 0;
    for (auto &&[i, VC] : VM) {
        auto &&[V, C] = VC;
        mV.block(0, vcols, V.rows(), V.cols()) = V;
        mC.block(0, ccols, C.rows(), C.cols()) = C.array() + vcols;
        blocks.push_back(vcols);
        vcols += V.cols();
        ccols += C.cols();
    }
    blocks.push_back(vcols);
    return std::tuple<RetVerts, RetCells, std::vector<int>>(mV, mC, blocks);
}
template<typename VDerived, typename CDerived>
auto separate_elements_unified(const Eigen::MatrixBase<VDerived> &V, const std::map<int, CDerived> &Cs) {
    return unify(separate_elements(V, Cs));
}
}// namespace mtao::geometry::mesh
