#pragma once
#include <range/v3/view/reverse.hpp>
namespace mtao::iterator {
template <typename T>
auto reverse(T&& container) {
    return ranges::view::reverse(container);
}
}  // namespace mtao::iterator
