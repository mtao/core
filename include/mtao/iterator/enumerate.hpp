#pragma once
#include <range/v3/view/indices.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/zip.hpp>

namespace mtao::iterator {
template <typename... T>
auto enumerate(T&&... v) {
    return ranges::views::zip(ranges::views::indices,
                                 std::forward<T>(v)...);
}
template <typename... T>
auto enumerate_offset(int start, T&&... v) {
    return enumerate(std::forward<T>(v)...) | ranges::views::drop(start);
}
}  // namespace mtao::iterator
