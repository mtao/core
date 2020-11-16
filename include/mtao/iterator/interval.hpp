#pragma once

#include "cycle.hpp"
#include "shell.hpp"
#include "zip.hpp"

namespace mtao::iterator {
namespace detail {

template <int... N, typename Container>
auto interval(std::integer_sequence<int, N...>, Container&& c) {
    return zip(shell(c.begin() + N, c.end())...);
}
template <int... N, typename Container>
auto cyclic_interval(std::integer_sequence<int, N...>, Container&& c) {
    return zip(c, cycle_with_offset(c, N + 1)...);
}
}  // namespace detail
template <int N, typename T>
auto interval(T&& v) {
    return detail::interval(std::make_integer_sequence<int, N>(),
                            std::forward<T>(v));
}
template <int N, typename T>
auto interval(std::initializer_list<T>&& v) {
    return detail::interval(std::make_integer_sequence<int, N>(), v);
}
// TODO: Cyclie it  needs to have zip where i can  shear the zip
template <int N, typename T>
auto cyclic_interval(T&& v) {
    return detail::cyclic_interval(std::make_integer_sequence<int, N - 1>(),
                                   std::forward<T>(v));
}
template <int N, typename T>
auto cyclic_interval(std::initializer_list<T>&& v) {
    return detail::cyclic_interval(std::make_integer_sequence<int, N - 1>(), v);
}
}  // namespace mtao::iterator
