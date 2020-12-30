#pragma once
#include <cstdlib>
#include <limits>
namespace mtao::iterator {
namespace detail {

    template<typename Index>
    struct range_iterator {
      public:
        range_iterator(int val, int inc) : m_val(val), m_inc(inc) {}

        bool operator==(const range_iterator &o) const { return m_val == o.m_val; }
        bool operator!=(const range_iterator &o) const { return m_val != o.m_val; }
        bool operator<(const range_iterator &o) const {
            return (m_inc > 0) ? (m_val < o.m_val) : (m_val > o.m_val);
        }

        range_iterator &operator++() {
            m_val += m_inc;
            return *this;
        }

        Index operator*() const { return m_val; }

      private:
        Index m_val, m_inc;
    };

    template<typename Index>
    struct range_container {
        range_container() = delete;
        range_container(range_container &&) = default;
        range_container(const range_container &) = default;
        range_container &operator=(range_container &&) = default;
        range_container &operator=(const range_container &) = default;

        using iterator = range_iterator<Index>;

        iterator begin() const { return iterator(m_start, m_inc); }
        iterator end() const {
            Index range = m_end - m_start;
            auto dr = std::lldiv(range, m_inc);
            if (dr.rem == 0) {
                return iterator(m_end, m_inc);
            } else {
                return iterator(m_start + m_inc * (dr.quot + 1), m_inc);
            }
        }

        range_container(Index a, Index b, Index c)
          : m_start(a), m_end(b), m_inc(c) {}

        range_container operator+(Index v) const {
            return range_container(m_start + v, m_end + v, m_inc);
        }
        range_container operator-(Index v) const {
            return range_container(m_start - v, m_end - v, m_inc);
        }

      private:
        Index m_start, m_end, m_inc;
    };

}// namespace detail
namespace internal {
    template<typename Index>
    constexpr Index largest() {
        if constexpr (std::numeric_limits<Index>::has_infinity) {
            return std::numeric_limits<Index>::infinity();
        } else {
            // These weird parens are to hide when windows.h defines a min/max macro
            // somewhere!
            return (std::numeric_limits<Index>::max)();
        }
    }
}// namespace internal
template<typename Index = int>
inline auto inf_range(Index a = 0) {
    return detail::range_container<Index>(a, internal::largest<Index>(), 1);
}

template<typename Index = int>
inline auto range(Index N = internal::largest<Index>()) {
    return detail::range_container<Index>(0, N, 1);
}
template<typename Index = int>
inline auto range(Index a, Index b, Index inc = 1) {
    return detail::range_container<Index>(a, b, inc);
}

}// namespace mtao::iterator
