#pragma once
#include <tuple>
#include <utility>

namespace mtao {
    namespace iterator {
        namespace detail {
            template <typename BeginIt, typename EndIt>
                struct shell_container;

            template <typename T, bool Expr =std::is_lvalue_reference_v<T>>
                struct choose_storage {
                    using value_type = T&;
                };
            template <typename T>
                struct choose_storage<T,false> {
                    using value_type = T;
                };
            template <typename T, int D>
                struct choose_storage<T[D],true> {
                    using value_type = detail::shell_container<T*,T*>;
                };
            template <typename T>
                struct choose_storage<const T,false> {
                    using value_type = const T;
                };
            template <typename T>
                struct choose_storage<const T,true> {
                    using value_type = const T&;
                };
            template <typename T>
                using choose_storage_t = typename choose_storage<T>::value_type;

            template <typename BeginIt, typename EndIt>
                struct shell_container {
                    public:
                        using iterator = BeginIt;
                        shell_container(BeginIt&& b, EndIt&& e): m_begin(b), m_end(e) {}
                        auto begin() { return m_begin; }
                        auto begin() const { return m_begin; }
                        auto end() { return m_end; }
                        auto end() const { return m_end; }
                    private:
                        const BeginIt m_begin;
                        const EndIt m_end;
                };
        }

        template <typename BeginIt, typename EndIt>
            auto shell(BeginIt&& b, EndIt&& e) {
                return detail::shell_container<BeginIt, EndIt>(std::forward<BeginIt>(b), std::forward<EndIt>(e));
            }
        template <typename Container>
            auto shell(Container&& c) {
                return shell(c.begin(),c.end());
            }
        template <typename T, int D>
            auto shell(const T c[D]) {
                return shell(c,c+D);
            }
        template <typename T, int D>
            auto shell(T c[D]) {
                return shell(c,c+D);
            }
    }


}
