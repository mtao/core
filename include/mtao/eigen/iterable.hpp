#pragma once

#include <Eigen/Dense>
#include "mtao/iterator/shell.hpp"
namespace mtao::eigen {
    template <typename Derived>
    auto iterable(Eigen::PlainObjectBase<Derived>& v) {
        return mtao::iterator::shell(v.data(),v.data()+v.size());
    }
    template <typename Derived>
    auto iterable(const Eigen::PlainObjectBase<Derived>& v) {
        return mtao::iterator::shell(v.data(),v.data()+v.size());
    }
}
