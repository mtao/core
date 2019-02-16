#pragma once
#include <utility>
#include <tuple>
#include <functional>


namespace mtao {
    namespace functional {
        namespace internal {
            template <typename Func, std::size_t... M, typename... Args>
                auto tuple_invoke_impl(Func&& f, const std::tuple<Args...>& t, std::integer_sequence<std::size_t,M...>) {
                    return std::invoke(std::forward<Func>(f),(std::get<M>(t))...);
                }
        }

        template <typename Func, typename... Args>
            auto tuple_invoke(Func&& f, const std::tuple<Args...>& t) {
               return internal::tuple_invoke_impl(std::forward<Func>(f),t,std::make_integer_sequence<std::size_t,sizeof...(Args)>());
            }
        template <typename Func, typename... Args>
            auto tuple_invoke(Func&& f) {
                return std::invoke(std::forward<Func>(f));
            }
    }
}
