#pragma once
#include <algorithm>
#include <tuple>

#include "mtao/algebra/sort_with_swap_counts.hpp"

namespace mtao::algebra {

template <SortType type = SortType::Bubble,
          typename ContainerType = std::vector<int>>
bool sort_with_permutation_sign_in_place(ContainerType& c) {
    return sort_with_swap_count_in_place<type>(c) % 2 == 0;
}
template <SortType type = SortType::Bubble,
          typename ContainerType = std::vector<int>>
std::tuple<std::decay_t<ContainerType>, bool> sort_with_permutation_sign(
    const ContainerType& c) {
    std::decay_t<ContainerType> c2 = c;
    bool sign = sort_with_permutation_sign_in_place<type>(c2);
    return {c2, sign};
}
}  // namespace mtao::algebra
