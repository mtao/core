#pragma once
#include "zip.hpp"
#include "range.hpp"

namespace mtao {
    namespace iterator {
    template <typename T>
        auto enumerate(T&& v, int start=0) {
            return zip(inf_range(start),std::forward<T>(v));
        }
    template <typename T>
        auto enumerate(std::initializer_list<T>&& v,int start=0) {
            return zip(inf_range(start),v);
        }
}
}
