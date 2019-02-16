#pragma once
#include "shell.hpp"
namespace mtao {
    namespace iterator {
        namespace detail {

            template <typename Type>
                class reverse_container {
                    public:
                    reverse_container() = delete;
                    reverse_container(reverse_container&&) = default;
                    reverse_container(const reverse_container&) = delete;
                    reverse_container& operator=(reverse_container&&) = delete;
                    reverse_container& operator=(const reverse_container&) = delete;
                    reverse_container(Type&& t): m_container(t) {}


                    auto begin() const { return m_container.rbegin(); }
                    auto end() const { return m_container.rend(); }
                    auto begin() { return m_container.rbegin(); }
                    auto end() { return m_container.rend(); }
                    private:
                    choose_storage_t<Type> m_container;
                };
        }
        template <typename T>
            auto reverse(T&& container) {
                return detail::reverse_container<T>(container);
            }
    }
}
