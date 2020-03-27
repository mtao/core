#pragma once
#include <set>
#include <tuple>
#include <vector>

#include "mtao/types.hpp"

namespace mtao::geometry::mesh {
// converts a collection of curves represented as pairs of indices in E into a
// collection of curves. By default it alwys generates closed curves but when
// closed_only=false it also can generate open and closed curves as long as
// vertices have valence <=2 This function returns a collection of <vector,bool>
// pairs where hte bool says whehter the returned curve is closed. if
// closed_only=true then the bool is always true
template <typename EDerived>
std::vector<std::tuple<std::vector<int>, bool>> edge_to_plcurves(
    const Eigen::MatrixBase<EDerived>& E, bool closed_only = true) {
    std::map<int, std::array<int, 2>> adj;
    for (int i = 0; i < E.cols(); ++i) {
        auto e = E.col(i);
        if (auto it = adj.find(e(0)); it == adj.end()) {
            adj[e(0)] = std::array<int, 2>{{i, -1}};
        } else {
            // it->second[1] = i;
            adj[e(0)][1] = i;
        }
        if (auto it = adj.find(e(1)); it == adj.end()) {
            adj[e(1)] = std::array<int, 2>{{i, -1}};
        } else {
            // it->second[0] = i;
            adj[e(1)][1] = i;
        }
    }

    // create an orientation by running around open edges and then random edges
    std::set<int> unseen_edges;
    std::set<int> open_edges;
    for (auto&& [k, v] : adj) {
        unseen_edges.emplace(k);
        if (v[0] < 0 || v[1] < 0) {
            open_edges.emplace(k);
        }
    }
    auto remove = [&](int idx) {
        if (auto it = open_edges.find(idx); it != open_edges.end()) {
            open_edges.erase(it);
        }
        if (auto it = unseen_edges.find(idx); it != unseen_edges.end()) {
            unseen_edges.erase(it);
        }
    };
    std::vector<std::tuple<std::vector<int>, bool>> ret;
    auto run = [&](const int start_vertex, const int start_edge) {
        auto& [vec, closedness] = ret.emplace_back();
        closedness = true;
        int vertex = start_vertex;
        int edge = start_edge;
        do {
            vec.emplace_back(vertex);
            auto&& e = E.col(edge);
            // step vertex
            if (e[0] == vertex) {
                vertex = e[1];
            } else {
                vertex = e[0];
            }
            remove(vertex);

            // step edge and swap it into the right order
            auto& de = adj[vertex];
            if (de[0] != edge) {
                std::swap(de[0], de[1]);
            }
            edge = de[1];
            // if we hit an end we can continue wrapping around or end,
            // depending on closed_only
            if (edge < 0) {
                if (closed_only) {
                    edge = de[0];
                } else {
                    closedness = false;
                    break;
                }
            }

        } while (vertex != start_vertex);
        if (vec.size() == 2 && !closed_only) {
            closedness = false;
        }
    };
    while (!open_edges.empty()) {
        int start_idx = *open_edges.begin();
        remove(start_idx);
        int start_edge;
        auto&& e = adj[start_idx];
        if (e[0] == -1) {
            start_edge = e[1];
        } else {
            start_edge = e[0];
        }
        run(start_idx, start_edge);
    }
    while (!unseen_edges.empty()) {
        int start_idx = *unseen_edges.begin();
        remove(start_idx);
        auto&& e = adj[start_idx];
        int start_edge = e[0];

        run(start_idx, start_edge);
    }
    return ret;
}  // namespace mtao::geometry::mesh
}  // namespace mtao::geometry::mesh
