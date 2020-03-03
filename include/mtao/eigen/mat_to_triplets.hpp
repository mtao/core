#pragma once
#include <Eigen/Sparse>

#include "mtao/eigen/type_info.hpp"

namespace mtao::eigen {
template <typename Derived>
auto mat_to_triplets(const Derived& V, typename Derived::Scalar thresh) {
    using Scalar = typename Derived::Scalar;
    std::vector<Eigen::Triplet<Scalar>> ret;

    ret.reserve(V.nonZeros());
    if constexpr (is_sparse(V)) {
        for (int k = 0; k < V.outerSize(); ++k) {
            for (typename Derived::InnerIterator it(V, k); it; ++it) {
                if (std::abs(it.value()) >= thresh) {
                    ret.emplace_back(it.row(), it.col(), it.value());
                }
            }
        }
    } else {
        constexpr bool RM = is_row_major(V);
        const int iCount = RM ? V.rows() : V.cols();
        const int jCount = RM ? V.cols() : V.rows();
        for (int i = 0; i < iCount; ++i) {
            for (int j = 0; j < jCount; ++j) {
                auto&& value = RM ? V(i, j) : V(j, i);
                if (std::abs(value) > thresh) {
                    if constexpr (RM) {
                        ret.emplace_back(i, j, value);
                    } else {
                        ret.emplace_back(j, i, value);
                    }
                }
            }
        }
    }
    return ret;
}
template <typename Derived>
auto mat_to_triplets(const Derived& V) {
    using Scalar = typename Derived::Scalar;
    std::vector<Eigen::Triplet<Scalar>> ret;

    ret.reserve(V.nonZeros());
    if constexpr (is_sparse(V)) {
        for (int k = 0; k < V.outerSize(); ++k) {
            for (typename Derived::InnerIterator it(V, k); it; ++it) {
                ret.emplace_back(it.row(), it.col(), it.value());
            }
        }
    } else {
        constexpr bool RM = is_row_major(V);
        const int iCount = RM ? V.rows() : V.cols();
        const int jCount = RM ? V.cols() : V.rows();
        for (int i = 0; i < iCount; ++i) {
            for (int j = 0; j < jCount; ++j) {
                auto&& value = RM ? V(i, j) : V(j, i);
                if (value != 0) {
                    if constexpr (RM) {
                        ret.emplace_back(i, j, value);
                    } else {
                        ret.emplace_back(j, i, value);
                    }
                }
            }
        }
    }
    return ret;
}
}  // namespace mtao::eigen
