#pragma once
#include "zip.hpp"
#include "range.hpp"

namespace mtao {
    namespace iterator {
        template <typename... T>
            auto enumerate_offset(int start, std::initializer_list<T>&&... v) {
                return zip(inf_range(start),v...);
            }
        template <typename... T>
            auto enumerate_offset(int start, T&&... v) {
                return zip(inf_range(start),std::forward<T>(v)...);
            }
        template <typename... T>
            auto enumerate(T&&... v) {
                return enumerate_offset(0,std::forward<T>(v)...);
            }
        template <typename... T>
            auto enumerate(std::initializer_list<T>&&... v) {
                return enumerate_offset(0,v...);
            }
    }
}
