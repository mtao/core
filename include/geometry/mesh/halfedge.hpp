#ifndef HALFEDGE_CELLCOMPLEX_H
#define HALFEDGE_CELLCOMPLEX_H
#include <vector>
#include "types.h"

namespace mtao { namespace geometry { namespace mesh {

struct cell_iterator;
struct vertex_iterator;
struct boundary_iterator;

class HalfEdgeMesh {
    public:
        enum class Index {VertexIndex=0,
            CellIndex=1,
            NextIndex=2,
            DualIndex=3,
            IndexEnd=4};
        using VecXi = mtao::VectorX<int>;
        template <int D> using ColVectorsi = mtao::ColVectors<int,D>;
        using Cells = mtao::MatrixX<int>;
        using Edges = ColVectorsi<int(Index::IndexEnd)>;
        using DatVec = mtao::Vector<int,int(Index::IndexEnd)>;


        struct HalfEdge;

        HalfEdgeMesh(const std::string& str);
        HalfEdgeMesh(const Cells& F);
        HalfEdgeMesh() = default;
        void construct(const Cells& F);

        auto vertex_indices() { return m_edges.row(int(Index::VertexIndex)); }
        auto cell_indices() { return m_edges.row(int(Index::CellIndex)); }
        auto next_indices() { return m_edges.row(int(Index::NextIndex)); }
        auto dual_indices() { return m_edges.row(int(Index::DualIndex)); }
        auto vertex_indices() const { return m_edges.row(int(Index::VertexIndex)); }
        auto cell_indices() const { return m_edges.row(int(Index::CellIndex)); }
        auto next_indices() const { return m_edges.row(int(Index::NextIndex)); }
        auto dual_indices() const { return m_edges.row(int(Index::DualIndex)); }

        int vertex_index(int idx) const { return vertex_indices()(idx); }
        int dual_index(int idx) const { return dual_indices()(idx); }
        int next_index(int idx) const { return next_indices()(idx); }
        int cell_index(int idx) const { return cell_indices()(idx); }
        const Edges& edges() const { return m_edges; }

        std::vector<int> cells() const;
        std::vector<int> vertices() const;
        std::vector<int> boundary() const;


        HalfEdge edge(int i) const;
        HalfEdge cell_edge(int i) const;
        HalfEdge vertex_edge(int i) const;

        std::vector<int> cell(int i) const;
        std::vector<int> dual_cell(int i) const;

        int size() const { return m_edges.cols(); }
        int boundary_size() const;
        int num_cells() const;
        int num_vertices() const;

        bool is_boundary(int index) const;
        bool is_boundary_vertex(int index) const;
        bool is_boundary_cell(int index) const;

    private:
        Edges m_edges;
        


};


struct HalfEdgeMesh::HalfEdge {
    public:
        using MeshType = HalfEdgeMesh;
        HalfEdge(const HalfEdge&) = default;
        HalfEdge(HalfEdge&&) = default;
        HalfEdge(const MeshType* cc, int idx=-1);
        int index() const { return m_index; }
        int vertex() const { return m_cc->vertex_index(index()); }
        int cell() const { return m_cc->cell_index(index()); }
        int next_index() const;
        int dual_index() const;
        HalfEdge get_next() const;
        HalfEdge get_dual() const;
        HalfEdge& next();
        HalfEdge& dual();

        operator int() const { return m_index; }
        operator bool() const { return m_index != -1; }
    private:
        const MeshType* m_cc;
        int m_index = -1;
};
namespace detail {
    void invalid_edge_warning();
}

template <typename Derived>
struct edge_iterator_base {
    public:
        using MeshType = HalfEdgeMesh;
        using HalfEdge = HalfEdgeMesh::HalfEdge;
        edge_iterator_base(const HalfEdge& he): m_he(he) {}
        edge_iterator_base(const MeshType* cc, int index): m_he(cc,index) {}
        edge_iterator_base(const MeshType& cc, int index): m_he(&cc,index) {}
        Derived& derived() { return *static_cast<Derived*>(this); }
        const Derived& derived() const { return *static_cast<const Derived*>(this); }

        static void increment(HalfEdge& he) { return Derived::increment(he); }

        HalfEdge start() const { return m_he; }

        void operator()(const std::function<void(const HalfEdge&)>& f) {
            auto it = start();
            do{
                f(it);
                increment(it);
            } while(it.index() != -1 && it.index() != m_he.index());
            if(it.index() == -1) {
                detail::invalid_edge_warning();
            }
        }

        void run_earlyout(const std::function<bool(const HalfEdge&)>& f) {
            auto it = start();
            do{
                if(!f(it)) {
                    return;
                }
                increment(it);
            } while(it.index() != -1 && it.index() != m_he.index());
            if(it.index() == -1) {
                detail::invalid_edge_warning();
            }
        }


    public:

        HalfEdge m_he;
};

struct cell_iterator: public edge_iterator_base<cell_iterator> {
    using Base = edge_iterator_base<cell_iterator>;
    using Base::Base;
    static void increment(HalfEdge& he);
};
struct vertex_iterator: public edge_iterator_base<vertex_iterator> {
    using Base = edge_iterator_base<vertex_iterator>;
    using Base::Base;
    static void increment(HalfEdge& he);
};
struct boundary_iterator: public edge_iterator_base<boundary_iterator> {
    using Base = edge_iterator_base<boundary_iterator>;
    using Base::Base;
    static void increment(HalfEdge& he);
};

}}}
#endif//HALFEDGE_CELLCOMPLEX_H
