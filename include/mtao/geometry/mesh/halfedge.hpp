#ifndef HALFEDGE_CELLCOMPLEX_H
#define HALFEDGE_CELLCOMPLEX_H
#include <Eigen/Geometry>
#include <array>
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

#include "mtao/data_structures/disjoint_set.hpp"
#include "mtao/geometry/cyclic_order.hpp"
#include "mtao/geometry/mesh/earclipping.hpp"
#include "mtao/types.hpp"

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
    using ColVectorsi = ColVectors<int, D>;
    using Cells = mtao::MatrixX<int>;
    using Edges = ColVectorsi<int(Index::IndexEnd)>;
    using DatVec = mtao::Vector<int, int(Index::IndexEnd)>;

    struct HalfEdge;

    HalfEdgeMesh(const std::string& str);
    HalfEdgeMesh(const Edges& E);
    static HalfEdgeMesh from_cells(const Cells& F,
                                   bool use_open_halfedges = false);
    static HalfEdgeMesh from_edges(
        const ColVectors<int, 2>& E);  // From non-dual edges
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

    // returns the directed halfedges that point away from each particular
    // vertex
    std::map<int, std::set<int>> vertex_edges_no_topology() const;

    void set_one_ring_adjacencies_quadcross(
        const std::vector<int>& ordered_edge_indices);
    template <typename TDerived = ColVectors<int, 2>>
        // get passed in half-edge indices
    void set_one_ring_adjacencies_quadcross(
        const std::set<int>& edges,
        const std::map<std::array<int, 2>, std::tuple<int, bool>>& tangent_map,
        const Eigen::MatrixBase<TDerived>& T);

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
    EmbeddedHalfEdgeMesh(const ColVectors<S, D>& V, Args&&... args)
        : HalfEdgeMesh(std::forward<Args>(args)...), m_vertices(V) {}

    static EmbeddedHalfEdgeMesh from_cells(const ColVectors<S, D>& V,
                                           const Cells& F,
                                           bool use_open_halfedges = false) {
        return EmbeddedHalfEdgeMesh(
            V, HalfEdgeMesh::from_cells(F, use_open_halfedges));
    }
    static EmbeddedHalfEdgeMesh from_edges(const ColVectors<S, D>& V,
                                           const ColVectors<int, 2>& E) {
        return EmbeddedHalfEdgeMesh(V, HalfEdgeMesh::from_edges(E));
    }

    ColVectors<int, 3> cells_triangulated() const;
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

    template <typename TDerived = ColVectors<int, 2>>
    void make_topology(const std::map<std::array<int, 2>, std::tuple<int, bool>>&
                           tangent_map = {},
                       const Eigen::MatrixBase<TDerived>& T = ColVectors<int,2>{});
    void tie_nonsimple_cells(const std::set<int>& cell_halfedges) const {
        return HalfEdgeMesh::tie_nonsimple_cells(m_vertices, cell_halfedges);
    }

    void tie_nonsimple_cells() {
        std::vector<int> C = cell_halfedges();
        return HalfEdgeMesh::tie_nonsimple_cells(
            m_vertices, std::set<int>(C.begin(), C.end()));
    }

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
    mtao::VectorX<int> get_cells(const ColVectors<S, D>& P) const;
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
    ColVectors<S, D> m_vertices;
};
namespace detail {
void invalid_edge_warning();
void invalid_cell_warning();
}  // namespace detail
}  // namespace mtao::geometry::mesh

#include "mtao/geometry/mesh/halfedge_impl.hpp"
#include "mtao/geometry/mesh/halfedge_iterator.hpp"

#endif  // HALFEDGE_CELLCOMPLEX_H
