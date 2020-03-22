#pragma once
#include <algorithm>
#include <tuple>

namespace mtao::algebra {

enum class SortType { Bubble, Insertion, Selection };

template <SortType type = SortType::Bubble,
          typename ContainerType = std::vector<int>>
size_t sort_with_swap_count_in_place(ContainerType& c);
template <SortType type = SortType::Bubble,
          typename ContainerType = std::vector<int>>
std::tuple<std::decay_t<ContainerType>, size_t> sort_with_swap_count(
    const ContainerType& c);

namespace internal {
// all of these are in place
template <typename ContainerType>
size_t bubble_sort_with_count(ContainerType& c) {
    if (c.size() < 2) {
        return 0;
    }
    size_t count = 0;
    for (size_t end = c.size() - 1; end > 0; --end) {
        for (size_t idx = 0; idx < end; ++idx) {
            if (c[idx] > c[idx + 1]) {
                std::swap(c[idx], c[idx + 1]);
                count++;
            }
        }
    }
    return count;
}
template <typename ContainerType>
size_t insertion_sort_with_count(ContainerType& c) {
    if (c.size() < 2) {
        return 0;
    }
    size_t count = 0;
    for (size_t start = 1; start < c.size(); ++start) {
        for (size_t idx = start; idx > 0 && c[idx - 1] > c[idx]; --idx) {
            std::swap(c[idx], c[idx - 1]);
            count++;
        }
    }
    return count;
}
template <typename ContainerType>
size_t selection_sort_with_count(ContainerType& c) {
    if (c.size() < 2) {
        return 0;
    }
    size_t count = 0;
    for (auto it = c.begin(); it != c.end(); ++it) {
        auto minit = std::min_element(it, c.end());
        if (it != minit) {
            std::swap(*it, *minit);
            count++;
        }
    }
    return count;
}

}  // namespace internal

template <SortType type, typename ContainerType>
size_t sort_with_swap_count_in_place(ContainerType& c) {
    if constexpr (type == SortType::Bubble) {
        return internal::bubble_sort_with_count(c);
    } else if constexpr (type == SortType::Insertion) {
        return internal::insertion_sort_with_count(c);
    } else if constexpr (type == SortType::Selection) {
        return internal::selection_sort_with_count(c);
    }
}
template <SortType type, typename ContainerType>
std::tuple<std::decay_t<ContainerType>, size_t> sort_with_swap_count(
    const ContainerType& c) {
    std::decay_t<ContainerType> c2 = c;
    size_t count = sort_with_swap_count_in_place<type>(c2);
    return {c2, count};
}
}  // namespace mtao::algebra
