#pragma once
#include <range/v3/view/indices.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/zip.hpp>

namespace mtao::iterator {
template <typename... T>
auto enumerate(T&&... v) {
    return ranges::v3::view::zip(ranges::v3::view::indices,
                                 std::forward<T>(v)...);
}
template <typename... T>
auto enumerate_offset(int start, T&&... v) {
    return enumerate(std::forward<T>(v)...) | ranges::v3::view::drop(start);
}
}  // namespace mtao::iterator
