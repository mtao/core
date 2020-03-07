#ifndef HALFEDGE_CELLCOMPLEX_H
#define HALFEDGE_CELLCOMPLEX_H
#include <Eigen/Geometry>
#include <array>
#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "mtao/data_structures/disjoint_set.hpp"
#include "mtao/geometry/mesh/earclipping.hpp"
#include "mtao/geometry/cyclic_order.hpp"
#include "mtao/types.h"

namespace mtao::geometry::mesh {

struct cell_iterator;
struct vertex_iterator;
struct boundary_iterator;

class HalfEdgeMesh {
   public:
    enum class Index {
        VertexIndex = 0,
        CellIndex = 1,
        NextIndex = 2,
        DualIndex = 3,
        IndexEnd = 4
    };
    using VecXi = mtao::VectorX<int>;
    template <int D>
    using ColVectorsi = mtao::ColVectors<int, D>;
    using Cells = mtao::MatrixX<int>;
    using Edges = ColVectorsi<int(Index::IndexEnd)>;
    using DatVec = mtao::Vector<int, int(Index::IndexEnd)>;

    struct HalfEdge;

    HalfEdgeMesh(const std::string& str);
    HalfEdgeMesh(const Edges& E);
    static HalfEdgeMesh from_cells(const Cells& F,
                                   bool use_open_halfedges = false);
    static HalfEdgeMesh from_edges(
        const mtao::ColVectors<int, 2>& E);  // From non-dual edges
    HalfEdgeMesh() = default;
    HalfEdgeMesh(const HalfEdgeMesh&) = default;
    HalfEdgeMesh(HalfEdgeMesh&&) = default;
    HalfEdgeMesh& operator=(const HalfEdgeMesh&) = default;
    HalfEdgeMesh& operator=(HalfEdgeMesh&&) = default;

    auto vertex_indices() { return m_edges.row(int(Index::VertexIndex)); }
    auto cell_indices() { return m_edges.row(int(Index::CellIndex)); }
    auto next_indices() { return m_edges.row(int(Index::NextIndex)); }
    auto dual_indices() { return m_edges.row(int(Index::DualIndex)); }
    auto vertex_indices() const { return m_edges.row(int(Index::VertexIndex)); }
    auto cell_indices() const { return m_edges.row(int(Index::CellIndex)); }
    auto next_indices() const { return m_edges.row(int(Index::NextIndex)); }
    auto dual_indices() const { return m_edges.row(int(Index::DualIndex)); }

    int vertex_index(int idx) const {
        if (idx < 0 || idx >= size())
            return -1;
        else
            return vertex_indices()(idx);
    }
    int dual_index(int idx) const {
        if (idx < 0 || idx >= size())
            return -1;
        else
            return dual_indices()(idx);
    }
    int next_index(int idx) const {
        if (idx < 0 || idx >= size())
            return -1;
        else
            return next_indices()(idx);
    }
    int cell_index(int idx) const {
        if (idx < 0 || idx >= size())
            return -1;
        else
            return cell_indices()(idx);
    }
    const Edges& edges() const { return m_edges; }

    // Returns a unique halfedge to access a particular element type
    std::vector<int> cell_halfedges() const;
    std::vector<int> vertex_halfedges() const;
    std::vector<int> boundary_halfedges() const;
    std::map<int, int> cell_halfedges_map() const;
    std::map<int, int> vertex_halfedges_map() const;
    std::map<int, std::set<int>> cell_collect_all_halfedges() const;

    std::vector<std::set<int>> cell_halfedges_multi_component() const;
    std::map<int, std::set<int>> cell_halfedges_multi_component_map() const;

    cell_iterator get_cell_iterator(const HalfEdge& he) const;
    vertex_iterator get_vertex_iterator(const HalfEdge& he) const;
    boundary_iterator get_boundary_iterator(const HalfEdge& he) const;
    cell_iterator get_cell_iterator(int he_idx) const;
    vertex_iterator get_vertex_iterator(int he_idx) const;
    boundary_iterator get_boundary_iterator(int he_idx) const;

    std::vector<std::vector<int>> cells() const;
    std::map<int, std::vector<int>> cells_map() const;
    std::map<int, std::set<std::vector<int>>> cells_multi_component_map() const;
    std::map<int, std::set<int>> halfedges_per_cell() const;
    std::vector<int> cell(int cell_index) const;
    std::vector<int> cell(const HalfEdge& e) const;
    std::vector<int> cell_he(int he_in_cell) const;

    std::map<int, std::set<int>> vertices_per_cell() const;

    std::vector<std::vector<int>> dual_cells() const;
    std::map<int, std::vector<int>> dual_cells_map() const;
    std::vector<int> dual_cell(int vertex_index) const;
    std::vector<int> dual_cell(const HalfEdge& e) const;
    std::vector<int> dual_cell_he(int he_in_cell) const;

    std::vector<int> one_ring(int vertex_idx) const;
    std::vector<int> one_ring(const HalfEdge& e) const;
    std::vector<int> one_ring_he(int he_pointing_to_vertex) const;

    HalfEdge edge(int i) const;
    HalfEdge cell_edge(int i) const;
    HalfEdge vertex_edge(int i) const;
    std::set<HalfEdge> cell_edges(int i) const;
    std::set<HalfEdge> vertex_edges(int i) const;

    // Edge {i,j} => halfedge pointing to j, where i is the dual
    std::map<std::array<int, 2>, int> edge_to_halfedge(
        bool use_next = false) const;

    int size() const { return m_edges.cols(); }
    int boundary_size() const;
    int num_cells() const;
    int num_vertices() const;

    bool is_boundary(int index) const;
    bool is_boundary_vertex(int index) const;
    bool is_boundary_cell(int index) const;

    HalfEdgeMesh submesh_from_cells(const std::set<int>& cell_indices) const;
    HalfEdgeMesh submesh_from_edges(const std::set<int>& edge_indices) const;
    HalfEdgeMesh submesh_from_vertices(const std::set<int>& edge_indices) const;

    // returns the directed halfedges that point away from each particular vertex
    std::map<int, std::set<int>> vertex_edges_no_topology() const;


    // atan based sorting
    // pass in a single vertex and the pertinent 
    template <typename Derived>
    void set_one_ring_adjacencies_cotan(const Eigen::MatrixBase<Derived>& V,
                                  const std::vector<int>& edges,
                                  bool stitch_ends = true);
    template <typename T>
    void set_one_ring_adjacencies_cotan(const std::map<T, int>& ordered_edge_indices,
                                  bool stitch_ends = true);
    template <typename T>
    void set_one_ring_adjacencies_cotan(
        const std::map<std::tuple<T, int>, int>& ordered_edge_indices,
        bool stitch_ends = true);

    // quadrant + cotan for sorting
    //template <typename Derived>
    //void set_one_ring_adjacencies_quadcross(const Eigen::MatrixBase<Derived>& V,
    //                              const std::vector<int>& edges,
    //                              bool stitch_ends = true);
    void set_one_ring_adjacencies_quadcross(const std::vector<int>& ordered_edge_indices);

    template <typename Derived, typename Derived2>
    typename Derived::Scalar winding_number(
        const Eigen::MatrixBase<Derived>& V, const HalfEdge& cell_edge,
        const Eigen::MatrixBase<Derived2>& p) const;
    template <typename Derived, typename Derived2>
    typename Derived::Scalar winding_number(
        const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_edge,
        const Eigen::MatrixBase<Derived2>& p) const;
    template <typename Derived>
    typename Derived::Scalar signed_area(const Eigen::MatrixBase<Derived>& V,
                                         const HalfEdge& cell_edge) const;
    template <typename Derived>
    typename Derived::Scalar signed_area(const Eigen::MatrixBase<Derived>& V,
                                         const std::set<int>& cell_edge) const;
    template <typename Derived>
    std::map<int, typename Derived::Scalar> signed_areas(
        const Eigen::MatrixBase<Derived>& V) const;
    template <typename Derived>
    typename Derived::Scalar area(const Eigen::MatrixBase<Derived>& V,
                                  const HalfEdge& cell_edge) const {
        return std::abs(signed_area(V, cell_edge));
    }
    template <typename Derived>
    typename Derived::Scalar area(const Eigen::MatrixBase<Derived>& V,
                                  const std::set<int>& cell_edge) const {
        return std::abs(signed_area(V, cell_edge));
    }

    template <typename Derived, typename Derived2>
    bool is_inside(const Eigen::MatrixBase<Derived>& V,
                   const std::set<int>& cell_edge,
                   const Eigen::MatrixBase<Derived2>& p) const;
    template <typename Derived>
    bool is_inside(const Eigen::MatrixBase<Derived>& V,
                   const std::set<int>& cell_edge,
                   const std::vector<int>& vertex_indices) const;

    template <typename Derived, typename Derived2>
    bool is_inside(const Eigen::MatrixBase<Derived>& V, int cell_edge,
                   const Eigen::MatrixBase<Derived2>& p) const;
    template <typename Derived, typename Derived2>
    bool is_inside(const Eigen::MatrixBase<Derived>& V,
                   const HalfEdge& cell_edge,
                   const Eigen::MatrixBase<Derived2>& p) const;
    template <typename Derived>
    bool is_inside(const Eigen::MatrixBase<Derived>& V, int cell_edge,
                   const std::vector<int>& vertex_indices) const;
    template <typename Derived>
    bool is_inside(const Eigen::MatrixBase<Derived>& V,
                   const HalfEdge& cell_edge,
                   const std::vector<int>& vertex_indices) const;

    template <typename Derived>
    void tie_nonsimple_cells(const Eigen::MatrixBase<Derived>& V,
                             const std::set<int>& cell_halfedges);
    template <typename Derived>
    void tie_nonsimple_cells(const Eigen::MatrixBase<Derived>& V) {
        std::vector<int> C = cell_halfedges();
        tie_nonsimple_cells(V, std::set<int>(C.begin(), C.end()));
    }

    void make_cells();  // assumes duals and cells nexts are set up, makes every
                        // element part of some cell

    bool is_island(int cell_idx) const;
    bool is_island(const HalfEdge& e) const;
    bool is_island_he(int he_in_cell) const;

    bool check_cell_validity() const;
    bool check_vertex_validity() const;
    bool check_halfedge_reachability() const;

   private:
    void construct(const Cells& F);
    void construct_open_halfedges(const Cells& F);
    void clear(size_t new_size = 0);
    void complete_boundary_cells();

   private:
    Edges m_edges;
};

struct HalfEdgeMesh::HalfEdge {
   public:
    using MeshType = HalfEdgeMesh;
    HalfEdge(const HalfEdge&) = default;
    HalfEdge(HalfEdge&&) = default;
    HalfEdge(const MeshType* cc, int idx = -1);
    int index() const { return m_index; }
    int vertex() const { return m_cc->vertex_index(index()); }
    int cell() const { return m_cc->cell_index(index()); }
    int next_index() const;
    int dual_index() const;
    HalfEdge get_next() const;
    HalfEdge get_dual() const;
    HalfEdge& next();
    HalfEdge& dual();

    explicit operator int() const { return m_index; }
    explicit operator bool() const { return m_index != -1; }
    bool operator<(const HalfEdge& other) const {
        return m_index < other.m_index;
    }

   private:
    const MeshType* m_cc;
    int m_index = -1;
};
template <typename S, int D>
class EmbeddedHalfEdgeMesh : public HalfEdgeMesh {
   public:
    using Vec = mtao::Vector<S, D>;

    EmbeddedHalfEdgeMesh() = default;
    EmbeddedHalfEdgeMesh(const EmbeddedHalfEdgeMesh&) = default;
    EmbeddedHalfEdgeMesh(EmbeddedHalfEdgeMesh&&) = default;
    EmbeddedHalfEdgeMesh& operator=(const EmbeddedHalfEdgeMesh&) = default;
    EmbeddedHalfEdgeMesh& operator=(EmbeddedHalfEdgeMesh&&) = default;
    ~EmbeddedHalfEdgeMesh() = default;
    template <typename... Args>
    EmbeddedHalfEdgeMesh(const mtao::ColVectors<S, D>& V, Args&&... args)
        : HalfEdgeMesh(std::forward<Args>(args)...), m_vertices(V) {}

    static EmbeddedHalfEdgeMesh from_cells(const mtao::ColVectors<S, D>& V,
                                           const Cells& F,
                                           bool use_open_halfedges = false) {
        return EmbeddedHalfEdgeMesh(
            V, HalfEdgeMesh::from_cells(F, use_open_halfedges));
    }
    static EmbeddedHalfEdgeMesh from_edges(const mtao::ColVectors<S, D>& V,
                                           const mtao::ColVectors<int, 2>& E) {
        return EmbeddedHalfEdgeMesh(V, HalfEdgeMesh::from_edges(E));
    }

    mtao::ColVectors<int, 3> cells_triangulated() const;
    auto V(int i) {
        assert(i >= 0);
        assert(i < nV());
        return m_vertices.col(i);
    }
    auto V(int i) const {
        assert(i >= 0);
        assert(i < nV());
        return m_vertices.col(i);
    }
    auto&& V() const { return m_vertices; }
    int nV() const { return m_vertices.cols(); }
    void make_topology();
    void tie_nonsimple_cells(const std::set<int>& cell_halfedges) const {
        return HalfEdgeMesh::tie_nonsimple_cells(m_vertices, cell_halfedges);
    }

    void tie_nonsimple_cells() {
        std::vector<int> C = cell_halfedges();
        return HalfEdgeMesh::tie_nonsimple_cells(
            m_vertices, std::set<int>(C.begin(), C.end()));
    }
    template <typename Derived>
    void set_one_ring_adjacencies_cotan(
        const Eigen::MatrixBase<Derived>& central_vertex,
        const std::set<int>& outward_facing_halfedges);

    template <typename Derived>
    void set_one_ring_adjacencies_quadcross(
        const Eigen::MatrixBase<Derived>& central_vertex,
        const std::set<int>& outward_facing_halfedges);

    template <typename Derived>
    bool is_inside(const std::set<int>& cell_edge,
                   const Eigen::MatrixBase<Derived>& p) const;
    bool is_inside(const std::set<int>& cell_edge,
                   const std::vector<int>& vertex_indices) const;

    template <typename Derived>
    bool is_inside(int cell_edge, const Eigen::MatrixBase<Derived>& p) const;
    bool is_inside(int cell_edge, const std::vector<int>& vertex_indices) const;

    template <typename Derived>
    bool is_inside(const HalfEdge& cell_edge,
                   const Eigen::MatrixBase<Derived>& p) const;
    bool is_inside(const HalfEdge& cell_edge,
                   const std::vector<int>& vertex_indices) const;
    template <typename Derived>
    int get_cell(const Eigen::MatrixBase<Derived>& p)
        const;  // returns the cell containing point p
    mtao::VectorX<int> get_cells(const mtao::ColVectors<S, D>& P) const;
    auto T(int idx) const {
        auto e = edge(idx);
        int a = e.vertex();
        int b = e.dual().vertex();
        return V(b) - V(a);
    }

    template <typename Derived>
    S winding_number(const std::set<int>& cell_edge,
                     const Eigen::MatrixBase<Derived>& p) const {
        return HalfEdgeMesh::winding_number(m_vertices, cell_edge, p);
    }
    S signed_area(const std::set<int>& cell_edge) const {
        return HalfEdgeMesh::signed_area(m_vertices, cell_edge);
    }
    S area(const std::set<int>& cell_edge) const {
        return HalfEdgeMesh::area(m_vertices, cell_edge);
    }
    template <typename Derived>
    S winding_number(const HalfEdge& cell_edge,
                     const Eigen::MatrixBase<Derived>& p) const {
        return HalfEdgeMesh::winding_number(m_vertices, cell_edge, p);
    }
    S signed_area(const HalfEdge& cell_edge) const {
        return HalfEdgeMesh::signed_area(m_vertices, cell_edge);
    }
    S area(const HalfEdge& cell_edge) const {
        return HalfEdgeMesh::area(m_vertices, cell_edge);
    }

    std::map<int, S> signed_areas() const {
        return HalfEdgeMesh::signed_areas(m_vertices);
    }

    std::set<std::tuple<S, int>> get_edge_angles(int eidx) const;

    std::map<int, std::set<int>> cell_containment() const;

   private:
    // creates next arrows
    mtao::ColVectors<S, D> m_vertices;
};
namespace detail {
void invalid_edge_warning();
void invalid_cell_warning();
}  // namespace detail

template <typename Derived>
struct edge_iterator_base {
   public:
    using MeshType = HalfEdgeMesh;
    using HalfEdge = HalfEdgeMesh::HalfEdge;
    edge_iterator_base(const HalfEdge& he) : m_he(he) {}
    edge_iterator_base(const MeshType* cc, int index) : m_he(cc, index) {}
    edge_iterator_base(const MeshType& cc, int index) : m_he(&cc, index) {}
    Derived& derived() { return *static_cast<Derived*>(this); }
    const Derived& derived() const {
        return *static_cast<const Derived*>(this);
    }

    static void increment(HalfEdge& he) { return Derived::increment(he); }

    HalfEdge start() const { return m_he; }

    void operator()(const std::function<void(const HalfEdge&)>& f) {
        auto it = start();
        do {
            f(it);
            increment(it);
        } while (it.index() != -1 && it.index() != m_he.index());
        if (it.index() == -1) {
            detail::invalid_edge_warning();
        }
    }

    void run_earlyout(const std::function<bool(const HalfEdge&)>& f) {
        auto it = start();
        do {
            if (!f(it)) {
                return;
            }
            increment(it);
        } while (it.index() != -1 && it.index() != m_he.index());
        if (it.index() == -1) {
            detail::invalid_edge_warning();
        }
    }

   public:
    HalfEdge m_he;
};

struct cell_iterator : public edge_iterator_base<cell_iterator> {
    using Base = edge_iterator_base<cell_iterator>;
    using Base::Base;
    static void increment(HalfEdge& he);
};
struct vertex_iterator : public edge_iterator_base<vertex_iterator> {
    using Base = edge_iterator_base<vertex_iterator>;
    using Base::Base;
    static void increment(HalfEdge& he);
};
struct boundary_iterator : public edge_iterator_base<boundary_iterator> {
    using Base = edge_iterator_base<boundary_iterator>;
    using Base::Base;
    static void increment(HalfEdge& he);
};

template <typename S, int D>
void EmbeddedHalfEdgeMesh<S, D>::make_topology() {
    static_assert(D == 2);
    // MAke edge connectivity
    auto e2v = vertex_edges_no_topology();

#ifdef _DEBUG
    for(auto&& [vidx, edges]: e2v) {
        for(auto&& [i,eidx]: mtao::iterator::enumerate(edges)) {
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

    int idx = 0;
#pragma omp parallel for
    for (idx = 0; idx < verts.size(); ++idx) {
        int vidx = verts[idx];
#ifdef MTAO_USE_ATAN2_CYCLIC_ORDERING
        set_one_ring_adjacencies_cotan(V(vidx), e2v.at(vidx));
#else
        set_one_ring_adjacencies_quadcross(V(vidx), e2v.at(vidx));
#endif
    }

#else
    for (auto&& [vidx, edges] : e2v) {
#ifdef MTAO_USE_ATAN2_CYCLIC_ORDERING
        set_one_ring_adjacencies_cotan(V(vidx), edges);
#else
        set_one_ring_adjacencies_quadcross(V(vidx), edges);
#endif
    }
#endif

    make_cells();
}
template <typename T>
void HalfEdgeMesh::set_one_ring_adjacencies_cotan(
    const std::map<std::tuple<T, int>, int>& ordered_edges, bool stitch_ends) {
    auto ni = next_indices();
    auto di = dual_indices();
    auto nit = ordered_edges.begin();
    if (ordered_edges.size() == 1) {
        int eidx = nit->second;
        ni(eidx) = di(eidx);
    } else {
        auto it = nit++;
        for (; nit != ordered_edges.end(); ++it, ++nit) {
            int nidx = nit->second;
            int idx = it->second;
            ni(idx) = di(nidx);
        }
        if (stitch_ends) {
            int idx = it->second;
            ni(idx) = di(ordered_edges.begin()->second);
        }
    }
}
template <typename T>
void HalfEdgeMesh::set_one_ring_adjacencies_cotan(
    const std::map<T, int>& ordered_edges, bool stitch_ends) {
    auto ni = next_indices();
    auto di = dual_indices();
    auto nit = ordered_edges.begin();
    if (ordered_edges.size() == 1) {
        int eidx = nit->second;
        ni(eidx) = di(eidx);
    } else {
        auto it = nit++;
        for (; nit != ordered_edges.end(); ++it, ++nit) {
            int nidx = nit->second;
            int idx = it->second;
            ni(idx) = di(nidx);
        }
        if (stitch_ends) {
            int idx = it->second;
            ni(idx) = di(ordered_edges.begin()->second);
        }
    }
}
template <typename Derived>
void HalfEdgeMesh::set_one_ring_adjacencies_cotan(const Eigen::MatrixBase<Derived>& V,
                                            const std::vector<int>& edges,
                                            bool stitch_ends) {
    assert(V.rows() == 2);
    using S = typename Derived::Scalar;
    std::map<S, int> edge_angles;
    if (edges.size() == 0) {
        return;
    }
    if (edges.size() == 1) {
        int eidx = *edges.begin();
        auto ni = next_indices();
        auto di = dual_indices();
        ni(eidx) = di(eidx);
        return;
        edge_angles[0] = eidx;
    } else {
        for (auto [i, eidx] : mtao::iterator::enumerate(edges)) {
            // HalfEdge e = edge(eidx);
            auto p = V.col(i);
            S ang = std::atan2(p.y(), p.x());
            edge_angles[ang] = eidx;
        }
        if (edges.size() != edge_angles.size()) {
            mtao::logging::warn() << "Duplicate vertex directions found when "
                                     "computing one ring adjacencies bb";
            std::map<std::tuple<S, int>, int> edge_angles;
            for (auto [i, eidx] : mtao::iterator::enumerate(edges)) {
                // HalfEdge e = edge(eidx);
                auto p = V.col(i);
                S ang = std::atan2(p.y(), p.x());
                edge_angles[std::make_tuple(ang, i)] = eidx;
            }
            HalfEdgeMesh::set_one_ring_adjacencies_cotan(edge_angles, stitch_ends);
        }
    }
    set_one_ring_adjacencies_cotan(edge_angles, stitch_ends);
}
template <typename S, int D>
template <typename Derived>
void EmbeddedHalfEdgeMesh<S, D>::set_one_ring_adjacencies_cotan(
    const Eigen::MatrixBase<Derived>& o, const std::set<int>& edges) {
    static_assert(D == 2);
    auto ni = next_indices();
    auto di = dual_indices();
    if (edges.size() == 1) {
        int eidx = *edges.begin();
        ni(eidx) = di(eidx);
    } else {
        std::map<S, int> edge_angles;

        std::transform(edges.begin(), edges.end(),
                       std::inserter(edge_angles, edge_angles.end()),
                       [&](int eidx) {
                           HalfEdge e = edge(eidx);
                           // int vertex = e.get_dual().vertex();
                           auto p = (V(e.get_dual().vertex()) - o);
                           S ang = std::atan2(p.y(), p.x());
                           return std::make_pair(ang, eidx);
                       });
        if (edges.size() != edge_angles.size()) {
            mtao::logging::warn() << "Duplicate vertex directions found when "
                                     "computing one ring adjacencies";
            std::map<std::tuple<S, int>, int> edge_angles;
            std::transform(
                edges.begin(), edges.end(),
                std::inserter(edge_angles, edge_angles.end()), [&](int eidx) {
                    HalfEdge e = edge(eidx);
                    int vertex = e.get_dual().vertex();
                    auto p = (V(e.get_dual().vertex()) - o);
                    S ang = std::atan2(p.y(), p.x());
                    return std::make_pair(std::make_tuple(ang, vertex), eidx);
                });
            HalfEdgeMesh::set_one_ring_adjacencies_cotan(edge_angles);
        } else {
            HalfEdgeMesh::set_one_ring_adjacencies_cotan(edge_angles);
        }
    }
}


//template <typename Derived>
//void HalfEdgeMesh::set_one_ring_adjacencies_quadcross(const Eigen::MatrixBase<Derived>& V,
//                                            const std::vector<int>& edges,
//                                            bool stitch_ends) {
//    assert(V.rows() == 2);
//    using S = typename Derived::Scalar;
//    std::map<S, int> edge_angles;
//    if (edges.size() == 0) {
//        return;
//    }
//    if (edges.size() == 1) {
//        int eidx = *edges.begin();
//        auto ni = next_indices();
//        auto di = dual_indices();
//        ni(eidx) = di(eidx);
//    } else {
//        auto ordered_edges = cyclic_order(V);
//        set_one_ring_adjacencies_quadcross(ordered_edges, stitch_ends);
//    }
//}

template <typename S, int D>
template <typename Derived>
void EmbeddedHalfEdgeMesh<S, D>::set_one_ring_adjacencies_quadcross(
    const Eigen::MatrixBase<Derived>& o, const std::set<int>& edges) {
    static_assert(D == 2);
#ifdef _DEBUG
        for(auto&& [i,eidx]: mtao::iterator::enumerate(edges)) {
            HalfEdge e = edge(eidx);
            // int vertex = e.get_dual().vertex();
            assert(e.get_dual().vertex() < nV());
        }
#endif
    auto ni = next_indices();
    auto di = dual_indices();
    if(edges.empty()) {
        return;
    } else if (edges.size() == 1) {
        int eidx = *edges.begin();
        ni(eidx) = di(eidx);
    } else {
        std::vector<int> edges_vec(edges.begin(),edges.end());
        mtao::ColVectors<S,D> VV(D,edges.size());
        for(auto&& [i,eidx]: mtao::iterator::enumerate(edges_vec)) {
            HalfEdge e = edge(eidx);
            // int vertex = e.get_dual().vertex();
            VV.col(i) = V(e.get_dual().vertex()) - o;
        }
        std::vector<int> order = cyclic_order(VV);
        std::transform(order.begin(),order.end(), order.begin(), [&](int idx) {
                return edges_vec.at(idx);
                });

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
mtao::ColVectors<int, 3> EmbeddedHalfEdgeMesh<S, D>::cells_triangulated()
    const {
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
    const mtao::ColVectors<S, D>& P) const {
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
#endif  // HALFEDGE_CELLCOMPLEX_H
