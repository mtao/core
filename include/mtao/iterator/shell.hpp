#pragma once
#include <memory>
#include <range/v3/view/subrange.hpp>
#include <tuple>
#include <utility>

namespace mtao::iterator {

template <typename BeginIt, typename EndIt>
auto shell(BeginIt&& b, EndIt&& e) {
    return ranges::make_subrange(b, e);
}
template <typename Container>
auto shell(Container&& c) {
    return shell(c.begin(), c.end());
}
template <typename T, int D>
auto shell(const T c[D]) {
    return shell(c, c + D);
}
template <typename T, int D>
auto shell(T c[D]) {
    return shell(c, c + D);
}
template <typename T>
auto shell(T* c, int D) {
    return shell(c, c + D);
}
template <typename T>
auto shell(const T* c, int D) {
    return shell(c, c + D);
}
template <typename T, typename Deleter>
auto shell(std::unique_ptr<T[], Deleter>& c, int D) {
    return shell(c.get(), D);
}
template <typename T, typename Deleter>
auto shell(const std::unique_ptr<T[], Deleter>& c, int D) {
    return shell(c.get(), D);
}
}  // namespace mtao::iterator

