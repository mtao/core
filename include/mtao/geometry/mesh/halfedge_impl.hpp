#pragma once
#include "mtao/geometry/mesh/halfedge.hpp"
#include "mtao/geometry/mesh/halfedge_iterator.hpp"
#include "mtao/types.hpp"
namespace mtao::geometry::mesh {
template <typename TDerived>
void HalfEdgeMesh::make_topology(
    const std::map<std::array<int, 2>, std::tuple<int, bool>>& tangent_map,
    const Eigen::MatrixBase<TDerived>& T) {
    // MAke edge connectivity
    auto e2v = vertex_edges_no_topology();

#ifdef _OPENMP
    std::vector<int> verts(e2v.size());
    std::transform(e2v.begin(), e2v.end(), verts.begin(),
                   [](auto&& pr) { return std::get<0>(pr); });

    size_t idx = 0;
#pragma omp parallel for
    for (idx = 0; idx < verts.size(); ++idx) {
        int vidx = verts[idx];
        HalfEdgeMesh::set_one_ring_adjacencies_quadcross(e2v.at(vidx),
                                                         tangent_map, T);
    }

#else
    for (auto&& [vidx, edges] : e2v) {
        HalfEdgeMesh::set_one_ring_adjacencies_quadcross(edges, tangent_map, T);
    }
#endif

    make_cells();
}
template <typename S, int D>
void EmbeddedHalfEdgeMesh<S, D>::make_topology() {
    static_assert(D == 2);
    // MAke edge connectivity
    auto e2v = vertex_edges_no_topology();

#ifdef _DEBUG
    for (auto&& [vidx, edges] : e2v) {
        for (auto&& [i, eidx] : mtao::iterator::enumerate(edges)) {
            HalfEdge e = edge(eidx);
            // int vertex = e.get_dual().vertex();
            assert(e.get_dual().vertex() < nV());
        }
    }
#endif
#ifdef _OPENMP
    std::vector<int> verts(e2v.size());
    std::transform(e2v.begin(), e2v.end(), verts.begin(),
                   [](auto&& pr) { return std::get<0>(pr); });

    size_t idx = 0;
#pragma omp parallel for
    for (idx = 0; idx < verts.size(); ++idx) {
        int vidx = verts[idx];
        set_one_ring_adjacencies_quadcross(V(vidx), e2v.at(vidx));
    }

#else
    for (auto&& [vidx, edges] : e2v) {
        set_one_ring_adjacencies_quadcross(V(vidx), edges);
    }
#endif

    make_cells();
}

template <typename S, int D>
template <typename Derived>
void EmbeddedHalfEdgeMesh<S, D>::set_one_ring_adjacencies_quadcross(
    const Eigen::MatrixBase<Derived>& o, const std::set<int>& edges) {
    static_assert(D == 2);
#ifdef _DEBUG
    for (auto&& [i, eidx] : mtao::iterator::enumerate(edges)) {
        HalfEdge e = edge(eidx);
        // int vertex = e.get_dual().vertex();
        assert(e.get_dual().vertex() < nV());
    }
#endif
    auto ni = next_indices();
    auto di = dual_indices();
    if (edges.empty()) {
        return;
    } else if (edges.size() == 1) {
        int eidx = *edges.begin();
        ni(eidx) = di(eidx);
    } else {
        std::vector<int> edges_vec(edges.begin(), edges.end());
        ColVectors<S, D> VV(D, edges.size());
        for (auto&& [i, eidx] : mtao::iterator::enumerate(edges_vec)) {
            HalfEdge e = edge(eidx);
            // int vertex = e.get_dual().vertex();
            VV.col(i) = V(e.get_dual().vertex()) - o;
        }
        std::vector<int> order = cyclic_order(VV);
        std::transform(order.begin(), order.end(), order.begin(),
                       [&](int idx) { return edges_vec.at(idx); });

        HalfEdgeMesh::set_one_ring_adjacencies_quadcross(order);
    }
}

template <typename TDerived>
void HalfEdgeMesh::set_one_ring_adjacencies_quadcross(
    const std::set<int>& edges,
    const std::map<std::array<int, 2>, std::tuple<int, bool>>& tangent_map,
    const Eigen::MatrixBase<TDerived>& T) {
    auto ni = next_indices();
    auto di = dual_indices();
    if (edges.empty()) {
        return;
    } else if (edges.size() == 1) {
        int eidx = *edges.begin();
        ni(eidx) = di(eidx);
    } else {
        using S = typename TDerived::Scalar;
        std::vector<int> edges_vec(edges.begin(), edges.end());
        ColVectors<S, 2> VV(2, edges.size());
        for (auto&& [i, eidx] : mtao::iterator::enumerate(edges_vec)) {
            HalfEdge e = edge(eidx);
            std::array<int, 2> ep{{e.get_dual().vertex(), e.vertex()}};
            std::sort(ep.begin(), ep.end());
            auto&& [tidx, flip_sign] = tangent_map.at(ep);
            // int vertex = e.get_dual().vertex();
            VV.col(i) = (flip_sign ? -1 : 1) * T.col(tidx);
        }
        std::vector<int> order = cyclic_order(VV);
        std::transform(order.begin(), order.end(), order.begin(),
                       [&](int idx) { return edges_vec.at(idx); });

        HalfEdgeMesh::set_one_ring_adjacencies_quadcross(order);
    }
}

template <typename Derived>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V, int cell_edge,
                             const std::vector<int>& vertex_indices) const {
    return is_inside(V, edge(cell_edge), vertex_indices);
}
template <typename Derived, typename Derived2>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V, int cell_edge,
                             const Eigen::MatrixBase<Derived2>& p) const {
    return is_inside(V, edge(cell_edge), p);
}

template <typename Derived, typename Derived2>
auto HalfEdgeMesh::winding_number(const Eigen::MatrixBase<Derived>& V,
                                  const HalfEdge& cell_edge,
                                  const Eigen::MatrixBase<Derived2>& p) const ->
    typename Derived::Scalar {
    using S = typename Derived::Scalar;

    auto edge = [](HalfEdge he) -> std::array<int, 2> {
        int i = he.vertex();
        he.next();
        int j = he.vertex();
        return {{i, j}};
    };
    S value = 0;
    get_cell_iterator(cell_edge)([&](auto&& e) {
        auto [i, j] = edge(e);
        auto a = V.col(i) - p;
        auto b = V.col(j) - p;
        S aa = std::atan2(a.y(), a.x());
        S ba = std::atan2(b.y(), b.x());
        S ang = ba - aa;
        if (ang > M_PI) {
            ang -= 2 * M_PI;
        } else if (ang <= -M_PI) {
            ang += 2 * M_PI;
        }
        value += ang;
    });
    return value;
}
template <typename Derived, typename Derived2>
auto HalfEdgeMesh::winding_number(const Eigen::MatrixBase<Derived>& V,
                                  const std::set<int>& cell_edge,
                                  const Eigen::MatrixBase<Derived2>& p) const ->
    typename Derived::Scalar {
    using S = typename Derived::Scalar;

    S ret = 0;
    for (auto&& ce : cell_edge) {
        ret += winding_number(V, edge(ce), p);
    }
    return ret;
}
template <typename Derived>
std::map<int, typename Derived::Scalar> HalfEdgeMesh::signed_areas(
    const Eigen::MatrixBase<Derived>& V) const {
    auto cem = cell_halfedges_multi_component_map();
    std::map<int, typename Derived::Scalar> ret;
    for (auto&& [c, hes] : cem) {
        if (c >= 0) {
            ret[c] = signed_area(V, hes);
        }
    }
    return ret;
}
template <typename Derived>
auto HalfEdgeMesh::signed_area(const Eigen::MatrixBase<Derived>& V,
                               const HalfEdge& cell_edge) const ->
    typename Derived::Scalar {
    using S = typename Derived::Scalar;

    S ret = 0;
    auto edge = [](HalfEdge he) -> std::array<int, 2> {
        int i = he.vertex();
        he.next();
        int j = he.vertex();
        return {{i, j}};
    };

    get_cell_iterator(cell_edge)([&](auto&& e) {
        auto [i, j] = edge(e);
        auto a = V.col(i);
        auto b = V.col(j);
        ret += a.x() * b.y() - a.y() * b.x();
    });
    return .5 * ret;
}
template <typename Derived>
auto HalfEdgeMesh::signed_area(const Eigen::MatrixBase<Derived>& V,
                               const std::set<int>& cell_edge) const ->
    typename Derived::Scalar {
    using S = typename Derived::Scalar;

    S ret = 0;
    for (auto&& ce : cell_edge) {
        ret += signed_area(V, edge(ce));
    }
    return ret;
}

template <typename Derived>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V,
                             const std::set<int>& cell_edge,
                             const std::vector<int>& vertex_indices) const {
    for (auto&& vi : vertex_indices) {
        if (!is_inside(V, cell_edge, V.col(vi))) {
            return false;
        }
    }
    return true;
}

template <typename Derived, typename Derived2>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V,
                             const std::set<int>& cell_edge,
                             const Eigen::MatrixBase<Derived2>& p) const {
    if constexpr (Derived2::ColsAtCompileTime == 1) {
        return std::abs(HalfEdgeMesh::winding_number(V, cell_edge, p)) >
               1;  // 1 is ok, looking for multiples of 2pi right?
    } else {
        for (int i = 0; i < p.cols(); ++i) {
            if (!is_inside(V, cell_edge, p.col(i))) {
                return false;
            }
        }
        return true;
    }
}
template <typename Derived>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V,
                             const HalfEdge& cell_edge,
                             const std::vector<int>& vertex_indices) const {
    for (auto&& vi : vertex_indices) {
        if (!is_inside(V, cell_edge, V.col(vi))) {
            return false;
        }
    }
    return true;
}

template <typename Derived, typename Derived2>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V,
                             const HalfEdge& cell_edge,
                             const Eigen::MatrixBase<Derived2>& p) const {
    if constexpr (Derived2::ColsAtCompileTime == 1) {
        return std::abs(HalfEdgeMesh::winding_number(V, cell_edge, p)) >
               1;  // 1 is ok, looking for multiples of 2pi right?
    } else {
        for (int i = 0; i < p.cols(); ++i) {
            if (!is_inside(V, cell_edge, p.col(i))) {
                return false;
            }
        }
        return true;
    }
}
template <typename S, int D>
template <typename Derived>
bool EmbeddedHalfEdgeMesh<S, D>::is_inside(
    const std::set<int>& cell_edge, const Eigen::MatrixBase<Derived>& p) const {
    return HalfEdgeMesh::is_inside(m_vertices, cell_edge, p);
}
template <typename S, int D>
bool EmbeddedHalfEdgeMesh<S, D>::is_inside(
    const std::set<int>& cell_edge,
    const std::vector<int>& vertex_indices) const {
    return HalfEdgeMesh::is_inside(m_vertices, cell_edge, vertex_indices);
}

template <typename S, int D>
template <typename Derived>
bool EmbeddedHalfEdgeMesh<S, D>::is_inside(
    int cell_edge, const Eigen::MatrixBase<Derived>& p) const {
    return is_inside(edge(cell_edge), p);
}
template <typename S, int D>
template <typename Derived>
bool EmbeddedHalfEdgeMesh<S, D>::is_inside(
    const HalfEdge& cell_edge, const Eigen::MatrixBase<Derived>& p) const {
    return HalfEdgeMesh::is_inside(m_vertices, cell_edge, p);
}
template <typename S, int D>
bool EmbeddedHalfEdgeMesh<S, D>::is_inside(
    int cell_edge, const std::vector<int>& vertex_indices) const {
    return is_inside(edge(cell_edge), vertex_indices);
}
template <typename S, int D>
bool EmbeddedHalfEdgeMesh<S, D>::is_inside(
    const HalfEdge& cell_edge, const std::vector<int>& vertex_indices) const {
    return HalfEdgeMesh::is_inside(m_vertices, cell_edge, vertex_indices);
}
template <typename S, int D>
ColVectors<int, 3> EmbeddedHalfEdgeMesh<S, D>::cells_triangulated() const {
    /*
    std::cout << "Cells: " << std::endl;
    for(auto&& c: cells()) {
        for(auto&& i: c) {
            std::cout << i << ",";
        }
        std::cout << std::endl;
    }
    std::cout << "^^^^^" << std::endl;
    */
    return mtao::geometry::mesh::earclipping(m_vertices, cells());
}
template <typename S, int D>
template <typename Derived>
int EmbeddedHalfEdgeMesh<S, D>::get_cell(
    const Eigen::MatrixBase<Derived>& p) const {
    static_assert(D == 2);
    auto C = cell_halfedges_multi_component_map();

    for (auto&& [c, hes] : C) {
        if (is_inside(hes, p)) {
            return c;
        }
    }
    return -1;
}
template <typename S, int D>
mtao::VectorX<int> EmbeddedHalfEdgeMesh<S, D>::get_cells(
    const ColVectors<S, D>& P) const {
    auto C = cell_halfedges();
    mtao::VectorX<int> indices(P.cols());

    auto edge = [](HalfEdge he) -> std::array<int, 3> {
        int i = he.vertex();
        he.next();
        int j = he.vertex();
        he.next();
        int k = he.vertex();
        he.next();
        return {{i, j, k}};
    };

    auto get_index = [&](auto&& p) -> int {
        for (size_t i = 0; i < C.size(); ++i) {
            bool inside = true;
            cell_iterator(this, C[i]).run_earlyout([&](auto&& e) -> bool {
                auto [i, j, k] = edge(e);
                auto a = V(i);
                auto b = V(j);
                auto c = V(k);
                auto ba = b - a;
                auto cb = c - b;
                auto pb = p - b;
                Vec n(-ba.y(), ba.x());
                inside = pb.dot(n) * cb.dot(n) >= 0;

                return inside;
            });
            if (inside) {
                return i;
            }
        }
        return -1;
    };

    for (int k = 0; k < P.cols(); ++k) {
        auto& idx = indices(k);
        auto p = P.col(k);
        idx = get_index(p);
    }
    return indices;
}

template <typename S, int D>
std::set<std::tuple<S, int>> EmbeddedHalfEdgeMesh<S, D>::get_edge_angles(
    int eidx) const {
    std::set<std::tuple<S, int>> edge_angles;

    vertex_iterator(this, eidx)([&](auto&& e) {
        auto t = T(e.index());
        S ang = std::atan2(t.y(), t.x());
        edge_angles.emplace({ang, e.index()});
    });
    /*
       std::transform(edges.begin(),edges.end(),std::inserter(edge_angles,edge_angles.end()),
       [&](int eidx) { HalfEdge e = edge(eidx); auto p =
       V(e.get_dual().vertex()) - o; T ang = std::atan2(p.y(),p.x()); return
       std::make_pair(ang,eidx);
       });
       */
    return edge_angles;
}

template <typename Derived>
void HalfEdgeMesh::tie_nonsimple_cells(const Eigen::MatrixBase<Derived>& V,
                                       const std::set<int>& cell_halfedges) {
    auto vols = signed_areas(V);
    std::map<int, std::set<int>> halfedge_partial_ordering;
    auto cells_map = this->cells_map();
    std::set<int> outer_hes;
    std::set<int> interior_hes;
    mtao::data_structures::DisjointSet<int> ds;
    ds.add_node(-1);
    for (auto&& he : cell_halfedges) {
        auto area = signed_area(V, edge(he));
        if (area > 0) {
            outer_hes.insert(he);
        } else if (area < 0) {
            interior_hes.insert(he);
        } else {
            // assert(area != 0);
            int ci = cell_index(he);
            ds.add_node(ci);
            ds.join(-1, ci);
        }
    }

    // the vertices that make each cell
    std::map<int, std::vector<int>> CM;
    std::map<int, std::set<int>> CMs;
    for (auto&& he : cell_halfedges) {
        auto&& v = CM[he] = cell_he(he);
        CMs[he] = std::set<int>(v.begin(), v.end());
    }

    for (auto&& he : cell_halfedges) {
        int cell = cell_index(he);
        ds.add_node(cell);
        ds.add_node(cell_index(dual_index(he)));
    }
    for (auto&& ohe : outer_hes) {
        int ocell = cell_index(ohe);
        // int docell = cell_index(dual_index(ohe));
        for (auto&& ihe : interior_hes) {
            int icell = cell_index(ihe);
            int dicell = cell_index(dual_index(ihe));
            if (ocell == icell || dicell == ocell) {
                continue;
            } else {
                if (is_inside(V, edge(ohe), CM[ihe])) {
                    auto&& potential_parent = CMs[ohe];
                    auto&& potential_child = CMs[ihe];
                    // if there's some random edge connecting these two we need
                    // to find it
                    if (std::includes(
                            potential_parent.begin(), potential_parent.end(),
                            potential_child.begin(), potential_child.end())) {
                        auto&& child = CM[ihe];
                        auto&& parent = CM[ohe];
                        std::set<std::array<int, 2>> edges;
                        for (int i = 0; i < parent.size(); ++i) {
                            std::array<int, 2> e;
                            e[0] = parent[i];
                            e[1] = parent[(i + 1) % parent.size()];
                            std::sort(e.begin(), e.end());
                            edges.emplace(e);
                        }
                        for (int i = 0; i < child.size(); ++i) {
                            std::array<int, 2> e;
                            e[0] = child[i];
                            e[1] = child[(i + 1) % child.size()];
                            std::sort(e.begin(), e.end());
                            if (edges.find(e) == edges.end()) {
                                auto v = (V.col(e[0]) + V.col(e[1])) / 2;
                                if (is_inside(V, edge(ohe), v)) {
                                    halfedge_partial_ordering[ohe].insert(ihe);
                                    break;
                                }
                            }
                        }
                    } else {
                        halfedge_partial_ordering[ohe].insert(ihe);
                    }
                }
            }
        }
    }

    std::map<int, std::set<int>> ordered_partial_ordering;
    std::set<int> seen_cells;
    for (auto&& [h, hs] : halfedge_partial_ordering) {
        ordered_partial_ordering[hs.size()].insert(h);
    }
    for (auto&& pr : ordered_partial_ordering) {
        auto&& hs = std::get<1>(pr);
        for (auto&& h : hs) {
            int cell = cell_index(h);
            auto&& children = halfedge_partial_ordering[h];

            for (auto&& che : children) {
                if (seen_cells.find(che) == seen_cells.end()) {
                    int ci = cell_index(che);
                    ds.join(cell, ci);
                    seen_cells.emplace(che);
                }
            }
        }
    }
    ds.reduce_all();
    {
        auto ci = cell_indices();
        for (int i = 0; i < size(); ++i) {
            int c = ci(i);
            if (ds.has_node(c)) {
                int root = ds.get_root(ci(i)).data;
                cell_indices()(i) = root;
            }
        }
    }
}
}  // namespace mtao::geometry::mesh
