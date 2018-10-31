#pragma once
#include "zip.hpp"
#include "range.hpp"

namespace mtao {
    namespace iterator {
    template <typename T>
        auto enumerate(T&& v) {
            return zip(range(),std::forward<T>(v));
        }
    template <typename T>
        auto enumerate(std::initializer_list<T>&& v) {
            return zip(range(),v);
        }
}
}
