#pragma once
#include <Eigen/SparseCore>
#include "mtao/type_utils.hpp"
#include <set>
#include <vector>
#include "mtao/iterator/enumerate.hpp"
#include <concepts>

namespace mtao::eigen {

template<typename T>
concept ContainCheckable = requires(const T a, typename T::key_type b) {
    { a.contains(b) }
    ->std::convertible_to<bool>;
};

template<typename Container>
std::set<typename Container::value_type> inverse_mask(typename Container::value_type size, const Container &masked_indices) {
    if constexpr (ContainCheckable<Container>) {

        std::set<typename Container::value_type> ret;
        for (typename Container::value_type idx(0); idx < size; ++idx) {
            if (!masked_indices.contains(idx)) {
                ret.emplace(idx);
            }
        }
        return ret;
    } else {
        std::set<typename Container::value_type> s{ masked_indices.begin(), masked_indices.end() };
        return inverse_mask(size, s);
    }
}

template<typename T, typename Container>
Eigen::SparseMatrix<T> mask_matrix(size_t size, const Container &masked_indices) {
    Eigen::SparseMatrix<T> R(size, size);
    R.reserve(masked_indices.size());
    if constexpr (ContainCheckable<Container>) {
        for (auto &&idx : masked_indices) {
            R.insert(idx, idx) = 1;
        }
    } else {
        std::vector<Eigen::Triplet<T>> t;
        t.reserve(masked_indices.size());
        std::transform(masked_indices.begin(), masked_indices.end(), std::back_inserter(t), [](auto &&idx) {
            return Eigen::Triplet<T>(idx, idx, 1);
        });
        R.setFromTriplets(t.begin(), t.end());
    }
    return R;
}

template<typename T, typename Container>
Eigen::SparseMatrix<T> projected_mask_matrix(size_t size, const Container &masked_indices) {
    Eigen::SparseMatrix<T> R(masked_indices.size(), size);
    R.reserve(masked_indices.size());
    if constexpr (ContainCheckable<Container>) {
        for (auto &&[lidx, idx] : iterator::enumerate(masked_indices)) {
            R.insert(lidx, idx) = 1;
        }
    } else {
        std::vector<Eigen::Triplet<T>> t;
        t.reserve(masked_indices.size());
        auto prs = iterator::enumerate(masked_indices);
        std::transform(prs.begin(), prs.end(), std::back_inserter(t), [](auto &&idx_pr) {
            auto [a, b] = idx_pr;
            return Eigen::Triplet<T>(a, b, 1);
        });
        R.setFromTriplets(t.begin(), t.end());
    }
    return R;
}


}// namespace mtao::eigen
