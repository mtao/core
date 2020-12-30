#pragma once
#include <tuple>
#include <utility>
#include <type_traits>
namespace mtao {

namespace internal {
    template<int... M, typename FuncType, typename... Args>
    auto static_loop(std::integer_sequence<int, M...>, FuncType &&func, Args &&... data) {
        if constexpr (std::is_void_v<std::result_of_t<FuncType(int, Args...)>>) {
            (func(M, std::forward<Args>(data)...), ...);
        } else {
            return std::make_tuple(func(M, std::forward<Args>(data)...)...);
        }
    }
}// namespace internal

template<int D, typename FuncType, typename... Args>
auto static_loop(FuncType &&func, Args &&... data) {
    return internal::static_loop(std::make_integer_sequence<int, D>(), func, std::forward<Args>(data)...);
}

template<int D, typename FuncType>
auto static_loop(FuncType &&func, Args &&... data) {
    return internal::static_loop(std::make_integer_sequence<int, D>(), func, std::forward<Args>(data)...);
}

}// namespace mtao
