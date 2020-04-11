#pragma once
#include <algorithm>
#include <map>
#include <set>
#include <tuple>

// https://en.wikipedia.org/wiki/Composition_of_relations
namespace mtao::algebra {

template <typename X, typename Y, typename Z>
auto composite(const std::set<std::tuple<X, Y>>& R,
               const std::set<std::tuple<Y, Z>>& S)
    -> std::set<std::tuple<X, Z>> {
    std::map<Y, std::set<X>> yx;
    for (auto&& [x, y] : R) {
        yx[y].emplace(x);
    }
    std::set<std::tuple<X, Z>> xz;
    for (auto&& [y, z] : S) {
        if (auto it = yx.find(y); it != yx.end()) {
            for (auto&& x : it->second) {
                xz.emplace(x, z);
            }
        }
    }

    return xz;
}

/*
// TODO: try to use this somehow; forgot the obvious fact that this sort of graph contraction eliminates odd-length curves
// something not quite related to composites, but perhaps useful / similar from
// an implementation perspective iterating the subsequent operator should
// converge
template <typename X>
auto composite_with_leftovers(const std::set<std::tuple<X, X>>& R,
                              const std::set<std::tuple<X, X>>& S)
    -> std::tuple<std::set<std::tuple<X,X>>, bool> {
    std::map<X, std::set<X>> yx;
    for (auto&& [x, y] : R) {
        yx[y].emplace(x);
    }
    std::set<std::tuple<X, X>> xz;
    std::set<std::tuple<X,X>> unused;
    std::copy(R.begin(), R.end(), std::inserter(unused, unused.end()));
    std::copy(S.begin(), S.end(), std::inserter(unused, unused.end()));

    for (auto&& [y, z] : S) {
        unused.erase({y,z});
        if (auto it = yx.find(y); it != yx.end()) {
            const X& y = it->first;
            std::cout << "used: " << y << std::endl;
            for (auto&& x : it->second) {
                unused.erase({x,y});
                xz.emplace(x, z);
            }
        }
    }
    bool done = unused.size() == 0;
    xz.merge(unused);

    return {std::move(xz), done};
}
*/
}  // namespace mtao::algebra
