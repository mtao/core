#pragma once
#if defined(MTAO_USE_RANGESv3)
#include <range/v3/view/drop.hpp>
#include <range/v3/view/indices.hpp>
#include <range/v3/view/zip.hpp>
#else
#include "mtao/iterator/range.hpp"
#include "mtao/iterator/zip.hpp"
#endif

namespace mtao::iterator {
#if defined(MTAO_USE_RANGESv3)
template <typename... T>
auto enumerate(T&&... v) {
    return ranges::views::zip(ranges::views::indices, std::forward<T>(v)...);
}
template <typename... T>
auto enumerate_offset(int start, T&&... v) {
    return enumerate(std::forward<T>(v)...) | ranges::views::drop(start);
}
#else
template <typename... T>
auto enumerate_offset(int start, std::initializer_list<T>&&... v) {
    return zip(inf_range(start), v...);
}
template <typename... T>
auto enumerate_offset(int start, T&&... v) {
    return zip(inf_range(start), std::forward<T>(v)...);
}
template <typename... T>
auto enumerate(T&&... v) {
    return enumerate_offset(0, std::forward<T>(v)...);
}
template <typename... T>
auto enumerate(std::initializer_list<T>&&... v) {
    return enumerate_offset(0, v...);
}
#endif
}  // namespace mtao::iterator
