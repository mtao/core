#ifndef COORD_H
#define COORD_H
#include <array>

namespace mtao {
    template <int N>
        class coord: public std::array<int,N> {//Coord is a lexicographicallysorted extension of std::array that also allows for a nicer constructor
            using Base = std::array<int,N>;
            using Base::Base;
            template <typename... Args>
            coord(Args... args): Base{{args...}} {
                static_assert(sizeof...(args) == N,"");
            }
                bool operator<(const coord& other) const {
                    for(int i=0; i < N; ++i) {
                        auto&& me = (*this)[i];
                        auto&& ot = other[i];
                        if(me < ot) {
                            return true;
                        } else if( me == ot) {
                            continue;
                        } else {
                            return false;
                        }
                    }
                    return false;
                }
                bool operator>(const coord& other) const {
                    return other < *this;
                }

                bool operator<=(const coord& other) const {
                    return !(other < *this);
                }
                bool operator>=(const coord& other) const {
                    return !(*this < other);
                }

        };
}

#endif//COORD_H
