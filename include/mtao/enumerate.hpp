#pragma once

#include <tuple>
#include <iostream>
namespace mtao {

    template <typename IteratorType>
        struct enumerator {
            using value_type = std::pair<int,decltype(*std::declval<IteratorType>())>;
            struct enum_iter {
                IteratorType it;
                int idx = 0;
                value_type operator*() { return value_type(idx,*it); }
                bool operator==(const enum_iter& o) { return it == o.it; }
                bool operator!=(const enum_iter& o) { return it != o.it; }
                enum_iter& operator++() { ++it; ++idx; return *this; }
            };

            enum_iter begin() const { return enum_iter{m_cur}; }
            enum_iter end() const { return enum_iter{m_end}; }

            enumerator(const IteratorType& c, const IteratorType& e): m_cur(c), m_end(e) {}
            private:
            IteratorType m_cur;
            IteratorType m_end;
        };

    template <typename T>
        auto enumerate(T& c) {
            return enumerator<typename T::iterator>(c.begin(),c.end());
        }
    template <typename T>
        auto enumerate(const T& c) {
            return enumerator<typename T::const_iterator>(c.begin(),c.end());
        }
    template <typename T>
        auto enumerate(const std::initializer_list<T>& c) {
            return enumerator<const T*>(c.begin(),c.end());
        }

}
