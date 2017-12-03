#ifndef EIGEN_UTILS_H
#define EIGEN_UTILS_H


#include "eigen/interweave.h"
#include "eigen/axial_subspace.h"

namespace mtao {
    namespace eigen {
        //For whtaever reason DenseBase doesn't contain unaryExpr so I have to write two copies
        //Also, i apparently have to eval this because the temporaries are invalidated somehow?
        template <typename Derived>
        auto finite(const Eigen::MatrixBase<Derived>& m) {
            using Scalar = typename Derived::Scalar;
            return m.unaryExpr([](const Scalar& v) -> Scalar {
                    return std::isfinite(v)?v:0;
                    }).eval();
        }
        template <typename Derived>
        auto finite(const Eigen::ArrayBase<Derived>& m) {
            using Scalar = typename Derived::Scalar;
            return m.unaryExpr([](const Scalar& v) -> Scalar {
                    return std::isfinite(v)?v:0;
                    }).eval();
        }
    }
}

#endif//EIGEN_UTILS_H
