#pragma once

namespace mtao {
    namespace iterator {
        namespace detail {
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
    }


}
