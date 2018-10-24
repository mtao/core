#pragma once
#include "mtao/geometry/prune_vertices.hpp"
#include <map>

namespace mtao { namespace geometry {
    //expects a mtao::vector<mtao::Vector<T,D>>
    template <typename Container 
        , typename Vec = typename Container::value_type
        , typename T = typename Vec::Scalar
        , int D = Vec::RowsAtCompileTime
        >
        std::tuple<Container,std::map<size_t,size_t>> prune(const Container& V, T eps = T(1e-8)) {
            Container ret_vec;
            KDTree<T,D> tree;
            std::map<size_t,size_t> remap;
            for(size_t i=0; i<V.size(); ++i) {
                remap[i] = tree.pruning_insertion(V[i],eps);
            }
            return {tree.points(),remap};
        }

}}
