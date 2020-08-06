#pragma once
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/take.hpp>

namespace mtao {
    namespace iterator {
        namespace detail {
        template <typename BeginIt, typename EndIt>
            struct cycle_container {
                public:
                    struct CycleIt {
                        public:
                            CycleIt(const cycle_container* c, const BeginIt& it): m_container(c), m_it(it) {}
                            auto&& operator*() {
                                return *m_it;
                            }
                            CycleIt& operator++() {
                                ++m_it;
                                if(m_it == m_container->end_it()) {
                                    m_it = m_container->begin_it();
                                }
                                return *this;
                            }
                            CycleIt& operator+=(size_t offset) {
                                m_it+=offset;
                                if(m_it == m_container->end_it()) {
                                    m_it = m_container->begin_it();
                                }
                                return *this;
                            }
                            bool operator!=(const CycleIt& other) const {
                                return m_it != other.m_it;
                            }
                            bool operator==(const CycleIt& other) const {
                                return m_it == other.m_it;
                            }
                        private:
                            const cycle_container* m_container;
                            BeginIt m_it;
                    };
                    using iterator = CycleIt;
                    cycle_container(BeginIt&& b, EndIt&& e): m_begin(b), m_end(e) {}
                    auto begin() { return CycleIt(this,m_begin); }
                    auto begin() const { return CycleIt(this,m_begin); }
                    auto end() { return CycleIt(this,m_end); }
                    auto end() const { return CycleIt(this,m_end); }

                    auto&& begin_it() const { return m_begin; }
                    auto&& end_it() const { return m_end; }
                private:
                    const BeginIt m_begin;
                    const EndIt m_end;
            };
        template <typename BeginIt, typename EndIt>
            struct cycle_container_with_offset: public cycle_container<BeginIt, EndIt> {
                using Base = cycle_container<BeginIt, EndIt>;
                using Base::Base;
                    cycle_container_with_offset(BeginIt&& b, EndIt&& e, size_t offset): Base(std::forward<BeginIt>(b),std::forward<EndIt>(e)), m_offset(offset) {}
                    auto begin() { return Base::begin()+=m_offset; }
                    auto begin() const { return Base::begin()+=m_offset; }
                    using Base::end;
                    size_t m_offset;
            };
        }

        template <typename BeginIt, typename EndIt>
            auto cycle(BeginIt&& b, EndIt&& e) {
                return detail::cycle_container<BeginIt, EndIt>(std::forward<BeginIt>(b), std::forward<EndIt>(e));
            }
        template <typename Container>
            auto cycle(Container&& c) {
                return ranges::views::cycle(c);
                //return cycle(c.begin(),c.end());
            }


        template <typename BeginIt, typename EndIt>
            auto cycle_with_offset(BeginIt&& b, EndIt&& e, size_t offset) {
                return detail::cycle_container_with_offset<BeginIt, EndIt>(std::forward<BeginIt>(b), std::forward<EndIt>(e), offset);
            }
        template <typename Container>
            auto cycle_with_offset(Container&& c, size_t offset) {
                return ranges::views::cycle(c) | ranges::views::take(offset);
                //return cycle_with_offset(c.begin(),c.end(), offset);
            }
    }


}
