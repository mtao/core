#pragma once

#include <array>
#include <tuple>

namespace mtao::utils {
namespace detail {
    template<int... N, typename IndexContainer>
    auto partial_sum(std::integer_sequence<int, N...>,
                     const IndexContainer &sizes) {
        using value_type = std::decay_t<decltype(std::get<0>(sizes))>;
        std::array<value_type, std::tuple_size_v<IndexContainer> + 1> arr;
        std::get<0>(arr) = 0;
        auto assign = [](value_type &a, value_type &&b) { a = std::move(b); };
        (assign(std::get<N + 1>(arr), std::get<N>(arr) + std::get<N>(sizes)), ...);
        return arr;
    }
}// namespace detail
template<typename IndexContainer>
auto partial_sum(const IndexContainer &sizes) {
    return detail::partial_sum(
      std::make_integer_sequence<int, std::tuple_size_v<IndexContainer>>{},
      sizes);
}
}// namespace mtao::utils
