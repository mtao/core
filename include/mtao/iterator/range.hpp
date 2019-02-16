#pragma once
#include <cstdlib>
namespace mtao {
    namespace iterator {
        namespace detail {

            struct range_iterator {
                public:
                    range_iterator(int val, int inc): m_val(val), m_inc(inc) {}


                    bool operator==(const range_iterator& o) const {return m_val == o.m_val;}
                    bool operator!=(const range_iterator& o) const {return m_val != o.m_val;}
                    bool operator<(const range_iterator& o) const{return (m_inc>0)?(m_val < o.m_val):(m_val > o.m_val);}

                    range_iterator& operator++() {m_val += m_inc; return *this;}

                    int operator*() const {return m_val;}

                private:
                    int m_val,m_inc;
            };


            struct range_container {


                range_container() = delete;
                range_container(range_container&&) = default;
                range_container(const range_container&) = default;
                range_container& operator=(range_container&&) = default;
                range_container& operator=(const range_container&) = default;

                using iterator = range_iterator;

                iterator begin() const { return iterator(m_start,m_inc);}
                iterator end() const { 
                    int range = m_end - m_start;
                    auto dr = std::lldiv(range,m_inc);
                    if(dr.rem == 0) {
                        return iterator(m_end,m_inc);
                    } else {
                        return iterator(m_start + m_inc * (dr.quot+1), m_inc);
                    }
                }

                range_container(int a, int b, int c): m_start(a), m_end(b), m_inc(c) {}


                private:
                int m_start, m_end, m_inc;

            };

        } 
        inline auto range(int N = std::numeric_limits<int>::has_infinity?std::numeric_limits<int>::infinity():std::numeric_limits<int>::max()) {
            return detail::range_container(0,N,1);
        }
        inline auto range(int a, int b, int inc = 1) {
            return detail::range_container(a,b,inc);
        }
    }
}
