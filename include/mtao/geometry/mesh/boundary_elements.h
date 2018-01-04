#pragma once

#include <mtao/types.h>
#include <Eigen/Sparse>
#include <vector>
#include <set>


namespace mtao { namespace geometry { namespace mesh {

    namespace internal {
    template <bool Primal,typename Derived>
        std::vector<std::set<int>> boundary_elements_impl(const Eigen::SparseCompressedBase<Derived>& B) {
            using II = typename Derived::InnerIterator;
            std::vector<std::set<int>> ret(Primal?B.cols():B.rows());
            for(int o = 0; o < B.outerSize(); ++o) {
                for(II it(B,o); it; ++it) {
                    if constexpr(Primal) {
                    ret[it.col()].insert(it.row());
                    } else {
                        ret[it.row()].insert(it.col());
                    }
                }
            }
            return ret;
        }
    }

    template <typename Derived>
        std::vector<std::set<int>> boundary_elements(const Eigen::SparseCompressedBase<Derived>& B) {
            return internal::boundary_elements_impl<true>(B);
        }

    template <typename Derived>
        std::vector<std::set<int>> coboundary_elements(const Eigen::SparseCompressedBase<Derived>& B) {
            return internal::boundary_elements_impl<false>(B);
        }

}}}

