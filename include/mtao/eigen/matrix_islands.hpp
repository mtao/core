#pragma once
#include "mtao/data_structures/disjoint_set.hpp"
#include "mtao/eigen/type_info.hpp"

namespace mtao::eigen {
template<typename Derived>
data_structures::DisjointSet<int> 
matrix_disjoint_sets(const Derived &V) {
    data_structures::DisjointSet<int> ds;
    for(int j = 0; j < std::max(V.rows(),V.cols()); ++j) {
        ds.add_node(j);
    }

    if constexpr (is_sparse<Derived>()) {
        for (int k = 0; k < V.outerSize(); ++k) {
            for (typename Derived::InnerIterator it(V, k); it; ++it) {
                ds.join(it.row(),it.col());
            }
        }
    } else {
        constexpr bool RM = is_row_major<Derived>();
        const int iCount = RM ? V.rows() : V.cols();
        const int jCount = RM ? V.cols() : V.rows();
        for (int i = 0; i < iCount; ++i) {
            for (int j = 0; j < jCount; ++j) {
                auto &&value = RM ? V(i, j) : V(j, i);
                if (value != 0) {
                    ds.join(i, j);
                }
            }
        }
    }
    ds.reduce_all();
    return ds;
}

template<typename Derived>
std::vector<std::set<int>>
matrix_islands(const Derived &V) {
    return matrix_disjoint_sets(V).as_flat_sets();

}
}
