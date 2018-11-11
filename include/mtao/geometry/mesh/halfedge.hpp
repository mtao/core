#ifndef HALFEDGE_CELLCOMPLEX_H
#define HALFEDGE_CELLCOMPLEX_H
#include <vector>
#include <map>
#include <set>
#include "mtao/types.h"

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
        HalfEdgeMesh(const Edges& E);
        static HalfEdgeMesh from_cells(const Cells& F);
        static HalfEdgeMesh from_edges(const mtao::ColVectors<int,2>& E);//From non-dual edges
        HalfEdgeMesh() = default;

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

        //Returns a unique halfedge to access a particular element type
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

        HalfEdgeMesh submesh_from_cells(const std::set<int>& cell_indices) const;
        HalfEdgeMesh submesh_from_edges(const std::set<int>& edge_indices) const;
        HalfEdgeMesh submesh_from_vertices(const std::set<int>& edge_indices) const;

    private:
        void construct(const Cells& F);
        void clear(size_t new_size = 0);
        std::map<int,std::set<int>> vertex_edges_no_topology() const;
        void make_cells();//assumes duals and cells nexts are set up, makes every element part of some cell
        void complete_boundary_cells();
    private:
        Edges m_edges;
        


};


template <typename T, int D>
class EmbeddedHalfEdgeMesh: public HalfEdgeMesh {
    public:


        template <typename... Args>
        EmbeddedHalfEdgeMesh(const mtao::ColVectors<T,D>& V, Args&& args): HalfEdgeMesh(std::forward<Args>(args)...), m_vertices(V) {}
        auto V(int idx) { return V.col(i); }
        auto V(int idx) const { return V.col(i); }
        void make_topology();
        auto T(int idx) const {
            auto e = edge(idx);
            int a = e.vertex();
            int b = e.dual().vertex();
            return V(b) - v(a);
        }
    private:
        //creates next arrows
        mtao::ColVectors<T,D> m_vertices;
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

        explicit operator int() const { return m_index; }
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

template <typename T, int D>
void EmbeddedHalfEdgeMesh<T,D>::make_topology() {

    if constexpr(D == 2) {
        //MAke edge connectivity
        auto ni = next_indices();
        auto di = dual_indices();
        auto e2v = vertex_edges_no_topology();
        for(auto&& [vidx,edges]: e2v) {
            if(edges.size() == 1) {
                int eidx = *edges.begin();
                ni(eidx) = di(eidx);
            } else {
                edge_angles = get_edge_angles(eidx);
                auto nit = edge_angles.begin();
                auto it = nit++;
                for(; nit != edge_angles.end(); ++it, ++nit) {
                    int nidx = nit->second;
                    int idx = it->second;
                    ni(idx) = di(nidx);
                }
                int idx = it->second;
                ni(idx) = di(edge_angles.begin()->second);
            }


        }

        make_cells();
    } else {
        static_assert(D == 2);
    }
}

template <typename T, int D>
std::map<T,int> EmbeddedHalfEdgeMesh<T,D>::get_edge_angles(int vidx) const {
    vertex_iterator(&hem,C[i])([&](auto&& e) {
            std::cout << e.cell() << " ";
            });
                auto o = V(vidx);
                std::map<T,int> edge_angles;

                std::transform(edges.begin(),edges.end(),std::inserter(edge_angles,edge_angles.end()), [&](int eidx) {
                        HalfEdge e = edge(eidx);
                        auto p = V(e.get_dual().vertex()) - o;
                        T ang = std::atan2(p.y(),p.x());
                        return std::make_pair(ang,eidx);
                        });

}}}
#endif//HALFEDGE_CELLCOMPLEX_H
