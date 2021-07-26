#pragma once
#include "mtao/type_utils.hpp"

namespace mtao::eigen {
// outputs the size of a STL container like we would use for an eigen type. In particular, for static size objects it returns their size and for dynamic size ones it outputs dynamic
template<typename T>
constexpr int container_size() {
    if constexpr (mtao::types::has_tuple_size<T>()) {
        return std::tuple_size<T>();
    } else {
        return Eigen::Dynamic;
    }
}
template<typename T>
constexpr int container_size(T &&) {
    return container_size<T>();
}
}// namespace mtao::eigen
