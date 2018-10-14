#pragma once
#include <utility>
#include <tuple>
#include <functional>


namespace mtao {
    namespace functional {
        namespace internal {
            template <typename Func, std::size_t... M, typename... Args>
                auto evaluate_tuple_impl(Func&& f, const std::tuple<Args...>& t, std::integer_sequence<std::size_t,M...>) {
                    return std::invoke(std::forward<Func>(f),(std::get<M>(t))...);
                }
        }

        template <typename Func, typename... Args>
            auto evaluate_tuple(Func&& f, const std::tuple<Args...>& t) {
               return internal::evaluate_tuple_impl(std::forward<Func>(f),t,std::make_integer_sequence<std::size_t,sizeof...(Args)>());
            }
        template <typename Func, typename... Args>
            auto evaluate_tuple(Func&& f) {
                return std::invoke(std::forward<Func>(f));
            }
    }
}
