#ifndef EIGEN_UTILS_H
#define EIGEN_UTILS_H


#include "mtao/eigen/interweave.h"
#include "mtao/eigen/stack.h"
#include "mtao/eigen/axial_subspace.h"

namespace mtao {
    namespace eigen {
        template <typename Derived>
        auto finite(const Eigen::ArrayBase<Derived>& m) {
            return m.isFinite();
        }
        template <typename Derived>
        auto finite(const Eigen::MatrixBase<Derived>& m) {
            return finite(m.array());
        }
    }
}

#endif//EIGEN_UTILS_H
