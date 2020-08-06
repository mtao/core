#pragma once
#include <numeric>

#include "mtao/geometry/mesh/compactify.hpp"
#include "mtao/iterator/enumerate.hpp"
#include "mtao/iterator/shell.hpp"
#include "mtao/type_utils.h"
#include "mtao/types.hpp"

namespace mtao::geometry::mesh {

namespace internal {
// either pass in itersatosr full of tuples {Vertices, Triangles} or tuples
// {Vertices,Triangles, colors}
template <typename BeginIt, typename EndIt>
auto stack_meshes(BeginIt beginit, EndIt endit) {
    using TupleType = std::decay_t<decltype(*beginit)>;
    using VType = std::tuple_element_t<0, TupleType>;
    using FType = std::tuple_element_t<1, TupleType>;
    using T = typename VType::Scalar;
    constexpr int D = VType::RowsAtCompileTime;
    constexpr bool has_color = std::tuple_size_v<TupleType> == 3;

    std::vector<int> sizes(std::distance(beginit, endit));
    std::transform(beginit, endit, sizes.begin(),
                   [](auto&& pr) { return std::get<0>(pr).cols(); });

    std::vector<int> offsets(sizes.size() + 1);
    offsets[0] = 0;
    std::partial_sum(sizes.begin(), sizes.end(), offsets.begin() + 1);

    std::vector<int> fsizes(sizes.size());
    std::transform(beginit, endit, fsizes.begin(),
                   [](auto&& pr) { return std::get<1>(pr).cols(); });
    std::vector<int> foffsets(fsizes.size() + 1);
    foffsets[0] = 0;
    std::partial_sum(fsizes.begin(), fsizes.end(), foffsets.begin() + 1);

    mtao::ColVectors<T, D> V(std::get<0>(*beginit).rows(), offsets.back());

    mtao::ColVectors<T, 4> C;

    if constexpr (has_color) {
        C.resize(4, V.cols());
    }
    FType F(std::get<1>(*beginit).rows(), foffsets.back());
    for (auto&& [idx, pr, off, foff] : mtao::iterator::enumerate(
             mtao::iterator::shell(beginit, endit), offsets, foffsets)) {
        if constexpr (has_color) {
            auto&& [v, f, c] = pr;
            V.block(0, off, v.rows(), v.cols()) = v;
            F.block(0, foff, f.rows(), f.cols()) = f.array() + off;
            if (c.cols() == 1) {
                C.block(0, off, 4, v.cols()).colwise() = c.col(0);
            } else {
                C.block(0, off, 4, v.cols()) = c;
            }
        } else {
            auto&& [v, f] = pr;
            V.block(0, off, v.rows(), v.cols()) = v;
            F.block(0, foff, f.rows(), f.cols()) = f.array() + off;
        }
    }
    if constexpr (has_color) {
        return std::make_tuple(V, F, C);
    } else {
        return std::make_tuple(V, F);
    }
}
}  // namespace internal
template <typename Container>
auto stack_meshes(const Container& container) {
    return internal::stack_meshes(container.begin(), container.end());
}

// eitehr container of {F} or container of {F,C}
template <typename VDerived, typename Container>
auto stack_meshes(const Eigen::MatrixBase<VDerived>& V,
                  const Container& container) {
    using T = typename VDerived::Scalar;
    constexpr int D = VDerived::RowsAtCompileTime;
    if constexpr (types::is_specialization_of_v<
                      std::tuple, typename Container::value_type>) {
        using IFType =
            typename std::tuple_element_t<0, typename Container::value_type>;
        using FType = mtao::ColVectors<int, IFType::RowsAtCompileTime>;
        std::vector<
            std::tuple<mtao::ColVectors<T, D>, FType, mtao::ColVectors<T, 4>>>
            tups;
        tups.reserve(container.size());
        for (auto&& [F, C] : container) {
            auto [v, f] = compactify(V, F);
            tups.emplace_back(std::move(v), std::move(f), C);
        }

        return internal::stack_meshes(tups.begin(), tups.end());
    } else {  // just a bunch of fs

        using FType =
            mtao::ColVectors<int, Container::value_type::RowsAtCompileTime>;
        std::vector<std::tuple<mtao::ColVectors<T, D>, FType>> tups;
        tups.reserve(container.size());
        for (auto&& F : container) {
            tups.emplace_back(compactify(V, F));
        }

        return internal::stack_meshes(tups.begin(), tups.end());
    }
}

}  // namespace mtao::geometry::mesh
