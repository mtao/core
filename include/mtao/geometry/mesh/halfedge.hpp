#ifndef HALFEDGE_CELLCOMPLEX_H
#define HALFEDGE_CELLCOMPLEX_H
#include <vector>
#include <map>
#include <set>
#include <utility>
#include "mtao/types.h"
#include <Eigen/Geometry>
#include "mtao/geometry/mesh/earclipping.hpp"
#include "mtao/data_structures/disjoint_set.hpp"
#include <iostream>
#include <array>

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
        static HalfEdgeMesh from_cells(const Cells& F, bool use_open_halfedges = false);
        static HalfEdgeMesh from_edges(const mtao::ColVectors<int,2>& E);//From non-dual edges
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

        int vertex_index(int idx) const { if(idx < 0 || idx >= size()) return -1; else return vertex_indices()(idx); }
        int dual_index(int idx) const {   if(idx < 0 || idx >= size()) return -1; else return dual_indices()(idx); }
        int next_index(int idx) const {   if(idx < 0 || idx >= size()) return -1; else return next_indices()(idx); }
        int cell_index(int idx) const {   if(idx < 0 || idx >= size()) return -1; else return cell_indices()(idx); }
        const Edges& edges() const { return m_edges; }

        //Returns a unique halfedge to access a particular element type
        std::vector<int> cell_halfedges() const;
        std::vector<int> vertex_halfedges() const;
        std::vector<int> boundary_halfedges() const;
        std::map<int,int> cell_halfedges_map() const;
        std::map<int,int> vertex_halfedges_map() const;

        std::vector<std::set<int>> cell_halfedges_multi_component() const;
        std::map<int,std::set<int>> cell_halfedges_multi_component_map() const;

        cell_iterator get_cell_iterator(const HalfEdge& he) const;
        vertex_iterator get_vertex_iterator(const HalfEdge& he) const;
        boundary_iterator get_boundary_iterator(const HalfEdge& he) const;
        cell_iterator get_cell_iterator(int he_idx) const;
        vertex_iterator get_vertex_iterator(int he_idx) const;
        boundary_iterator get_boundary_iterator(int he_idx) const;

        std::vector<std::vector<int>> cells() const;
        std::map<int,std::vector<int>> cells_map() const;
        std::map<int,std::set<std::vector<int>>> cells_multi_component_map() const;
        std::map<int,std::set<int>> halfedges_per_cell() const;
        std::vector<int> cell(int cell_index) const;
        std::vector<int> cell(const HalfEdge& e) const;
        std::vector<int> cell_he(int he_in_cell) const;


        std::map<int,std::set<int>> vertices_per_cell() const;

        std::vector<std::vector<int>> dual_cells() const;
        std::map<int,std::vector<int>> dual_cells_map() const;
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

        //Edge {i,j} => halfedge pointing to j, where i is the dual
        std::map<std::array<int,2>,int> edge_to_halfedge(bool use_next = false) const;

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
        std::map<int,std::set<int>> vertex_edges_no_topology() const;
        template <typename Derived>
            void set_one_ring_adjacencies(const Eigen::MatrixBase<Derived>& V, const std::vector<int>& edges, bool stitch_ends = true);
        template <typename T>
            void set_one_ring_adjacencies(const std::map<T,int>& ordered_edge_indices, bool stitch_ends = true);

        template <typename Derived, typename Derived2>
            typename Derived::Scalar winding_number(const Eigen::MatrixBase<Derived>& V, const HalfEdge& cell_edge, const Eigen::MatrixBase<Derived2>& p) const;
        template <typename Derived, typename Derived2>
            typename Derived::Scalar winding_number(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_edge, const Eigen::MatrixBase<Derived2>& p) const;
        template <typename Derived>
            typename Derived::Scalar signed_area(const Eigen::MatrixBase<Derived>& V, const HalfEdge& cell_edge) const;
        template <typename Derived>
            typename Derived::Scalar signed_area(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_edge) const;
        template <typename Derived>
            std::map<int,typename Derived::Scalar> signed_areas(const Eigen::MatrixBase<Derived>& V) const;
        template <typename Derived>
            typename Derived::Scalar area(const Eigen::MatrixBase<Derived>& V, const HalfEdge& cell_edge) const { return std::abs(signed_area(V,cell_edge)); }
        template <typename Derived>
            typename Derived::Scalar area(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_edge) const { return std::abs(signed_area(V,cell_edge)); }



        template <typename Derived, typename Derived2>
            bool is_inside(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_edge, const Eigen::MatrixBase<Derived2>& p) const;
        template <typename Derived>
            bool is_inside(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_edge, const std::vector<int>& vertex_indices) const;

        template <typename Derived, typename Derived2>
            bool is_inside(const Eigen::MatrixBase<Derived>& V, int cell_edge, const Eigen::MatrixBase<Derived2>& p) const;
        template <typename Derived, typename Derived2>
            bool is_inside(const Eigen::MatrixBase<Derived>& V, const HalfEdge& cell_edge, const Eigen::MatrixBase<Derived2>& p) const;
        template <typename Derived>
            bool is_inside(const Eigen::MatrixBase<Derived>& V, int cell_edge, const std::vector<int>& vertex_indices) const;
        template <typename Derived>
            bool is_inside(const Eigen::MatrixBase<Derived>& V, const HalfEdge& cell_edge, const std::vector<int>& vertex_indices) const;

        template <typename Derived>
            void tie_nonsimple_cells(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_halfedges) ;
        template <typename Derived>
            void tie_nonsimple_cells(const Eigen::MatrixBase<Derived>& V) {
                std::vector<int> C = cell_halfedges();
                return tie_nonsimple_cells(V,std::set<int>(C.begin(),C.end()));
            }

        void make_cells();//assumes duals and cells nexts are set up, makes every element part of some cell

        bool is_island(int cell_idx) const;
        bool is_island(const HalfEdge& e) const;
        bool is_island_he(int he_in_cell) const;

        bool check_cell_validity() const;
        bool check_vertex_validity() const;
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
        explicit operator bool() const { return m_index != -1; }
        bool operator<(const HalfEdge& other) const {
            return m_index < other.m_index;
        }

    private:
        const MeshType* m_cc;
        int m_index = -1;
};
template <typename S, int D>
class EmbeddedHalfEdgeMesh: public HalfEdgeMesh {
    public:

        using Vec = mtao::Vector<S,D>;

        EmbeddedHalfEdgeMesh() = default;
        EmbeddedHalfEdgeMesh(const EmbeddedHalfEdgeMesh&) = default;
        EmbeddedHalfEdgeMesh(EmbeddedHalfEdgeMesh&&) = default;
        EmbeddedHalfEdgeMesh& operator=(const EmbeddedHalfEdgeMesh&) = default;
        EmbeddedHalfEdgeMesh& operator=(EmbeddedHalfEdgeMesh&&) = default;
        ~EmbeddedHalfEdgeMesh() = default;
        template <typename... Args>
        EmbeddedHalfEdgeMesh(const mtao::ColVectors<S,D>& V, Args&&... args): HalfEdgeMesh(std::forward<Args>(args)...), m_vertices(V) {}

        static EmbeddedHalfEdgeMesh from_cells(const mtao::ColVectors<S,D>& V, const Cells& F, bool use_open_halfedges = false) {
            return EmbeddedHalfEdgeMesh(V,HalfEdgeMesh::from_cells(F, use_open_halfedges)); 
        }
        static EmbeddedHalfEdgeMesh from_edges(const mtao::ColVectors<S,D>& V, const mtao::ColVectors<int,2>& E) {
            return EmbeddedHalfEdgeMesh(V,HalfEdgeMesh::from_edges(E));
}


        mtao::ColVectors<int,3> cells_triangulated() const;
        auto V(int i) { return m_vertices.col(i); }
        auto V(int i) const { return m_vertices.col(i); }
        void make_topology();
        void tie_nonsimple_cells(const std::set<int>& cell_halfedges) const { return HalfEdgeMesh::tie_nonsimple_cells(m_vertices,cell_halfedges); }

        void tie_nonsimple_cells() {
            std::vector<int> C = cell_halfedges();
            return HalfEdgeMesh::tie_nonsimple_cells(m_vertices,std::set<int>(C.begin(),C.end()));
        }
        template <typename Derived>
        void set_one_ring_adjacencies(const Eigen::MatrixBase<Derived>& central_vertex, const std::set<int>& outward_facing_halfedges);

        template <typename Derived>
        bool is_inside(const std::set<int>&  cell_edge, const Eigen::MatrixBase<Derived>& p) const ;
        bool is_inside(const std::set<int>&  cell_edge, const std::vector<int>& vertex_indices) const ;

        template <typename Derived>
        bool is_inside(int cell_edge, const Eigen::MatrixBase<Derived>& p) const ;
        bool is_inside(int cell_edge, const std::vector<int>& vertex_indices) const ;

        template <typename Derived>
        bool is_inside(const HalfEdge& cell_edge, const Eigen::MatrixBase<Derived>& p) const ;
        bool is_inside(const HalfEdge& cell_edge, const std::vector<int>& vertex_indices) const ;
        template <typename Derived>
        int get_cell(const Eigen::MatrixBase<Derived>& p) const ;
        mtao::VectorX<int> get_cells(const mtao::ColVectors<S,D>& P) const;
        auto T(int idx) const {
            auto e = edge(idx);
            int a = e.vertex();
            int b = e.dual().vertex();
            return V(b) - V(a);
        }

template <typename Derived>
            S winding_number(const std::set<int>& cell_edge, const Eigen::MatrixBase<Derived>& p) const {
                return HalfEdgeMesh::winding_number(m_vertices,cell_edge,p);
            }
        S signed_area(const std::set<int>& cell_edge) const { return HalfEdgeMesh::signed_area(m_vertices,cell_edge); }
        S area(const std::set<int>& cell_edge) const { return HalfEdgeMesh::area(m_vertices,cell_edge); }
template <typename Derived>
            S winding_number(const HalfEdge& cell_edge, const Eigen::MatrixBase<Derived>& p) const {
                return HalfEdgeMesh::winding_number(m_vertices,cell_edge,p);
            }
        S signed_area(const HalfEdge& cell_edge) const { return HalfEdgeMesh::signed_area(m_vertices,cell_edge); }
        S area(const HalfEdge& cell_edge) const { return HalfEdgeMesh::area(m_vertices,cell_edge); }

std::map<int,S> signed_areas() const { return HalfEdgeMesh::signed_areas(m_vertices); }

        std::set<std::tuple<S,int>> get_edge_angles(int eidx) const;

        std::map<int,std::set<int>> cell_containment() const;
    private:
        //creates next arrows
        mtao::ColVectors<S,D> m_vertices;
};
namespace detail {
    void invalid_edge_warning();
    void invalid_cell_warning();
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

template <typename S, int D>
void EmbeddedHalfEdgeMesh<S,D>::make_topology() {

    static_assert(D == 2);
    //MAke edge connectivity
    auto e2v = vertex_edges_no_topology();
    for(auto&& [vidx,edges]: e2v) {
        set_one_ring_adjacencies(V(vidx),edges);
    }

    make_cells();
}
template <typename T>
void HalfEdgeMesh::set_one_ring_adjacencies(const std::map<T,int>& ordered_edges, bool stitch_ends) {

    auto ni = next_indices();
    auto di = dual_indices();
    auto nit = ordered_edges.begin();
    if(ordered_edges.size() == 1) {
        int eidx = nit->second;
        ni(eidx) = di(eidx);
    } else {
        auto it = nit++;
        for(; nit != ordered_edges.end(); ++it, ++nit) {
            int nidx = nit->second;
            int idx = it->second;
            ni(idx) = di(nidx);
        }
        if(stitch_ends) {
            int idx = it->second;
            ni(idx) = di(ordered_edges.begin()->second);
        }
    }
}
template <typename Derived>
void HalfEdgeMesh::set_one_ring_adjacencies(const Eigen::MatrixBase<Derived>& V, const std::vector<int>& edges, bool stitch_ends) {
    assert(V.rows() == 2);
    using S = typename Derived::Scalar;
    std::map<S,int> edge_angles;
    if(edges.size() == 0) {
        return;
    } if(edges.size() == 1) {

        int eidx = *edges.begin();
        auto ni = next_indices();
        auto di = dual_indices();
        ni(eidx) = di(eidx);
        return;
        edge_angles[0] = eidx;
    } else {

        for(auto [i,eidx]: mtao::iterator::enumerate(edges)) {
            HalfEdge e = edge(eidx);
            auto p = V.col(i);
            S ang = std::atan2(p.y(),p.x());
            edge_angles[ang] = eidx;
        }
    }
    /*
    for(auto&& [t,i]: edge_angles) {
        std::cout << i << "(" << t << ") ";
    } 
    std::cout << std::endl;
    */
    set_one_ring_adjacencies(edge_angles,stitch_ends);
}
template <typename S, int D>
template <typename Derived>
void EmbeddedHalfEdgeMesh<S,D>::set_one_ring_adjacencies(const Eigen::MatrixBase<Derived>& o, const std::set<int>& edges) {
    static_assert(D == 2);
    auto ni = next_indices();
    auto di = dual_indices();
    if(edges.size() == 1) {
        int eidx = *edges.begin();
        ni(eidx) = di(eidx);
    } else {
        std::map<S,int> edge_angles;

        std::transform(edges.begin(),edges.end(),std::inserter(edge_angles,edge_angles.end()), [&](int eidx) {
                HalfEdge e = edge(eidx);
                auto p = V(e.get_dual().vertex()) - o;
                S ang = std::atan2(p.y(),p.x());
                return std::make_pair(ang,eidx);
                });
        HalfEdgeMesh::set_one_ring_adjacencies(edge_angles);
    }
}

template <typename Derived>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V, int cell_edge, const std::vector<int>& vertex_indices) const {
    return is_inside(V,edge(cell_edge),vertex_indices);
}
template <typename Derived, typename Derived2>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V, int cell_edge, const Eigen::MatrixBase<Derived2>& p) const {
    return is_inside(V,edge(cell_edge),p);
}


template <typename Derived, typename Derived2>
auto HalfEdgeMesh::winding_number(const Eigen::MatrixBase<Derived>& V, const HalfEdge& cell_edge, const Eigen::MatrixBase<Derived2>& p) const -> typename Derived::Scalar {
    using S = typename Derived::Scalar;

    auto edge = [](HalfEdge he) -> std::array<int,2> {
        int i = he.vertex(); he.next(); 
        int j = he.vertex();
        return {{i,j}};

    };
    S value = 0;
    get_cell_iterator(cell_edge)([&](auto&& e){
            auto [i,j] = edge(e);
            auto a = V.col(i) - p;
            auto b = V.col(j) - p;
            S aa = std::atan2(a.y(),a.x());
            S ba = std::atan2(b.y(),b.x());
            S ang = ba - aa;
            if(ang > M_PI) {
            ang -= 2 * M_PI;
            } else if(ang <= -M_PI) {
            ang += 2*M_PI;
            }
            value += ang;

            });
    return value;

}
template <typename Derived, typename Derived2>
auto HalfEdgeMesh::winding_number(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_edge, const Eigen::MatrixBase<Derived2>& p) const -> typename Derived::Scalar {
    using S = typename Derived::Scalar;

    S ret = 0;
    for(auto&& ce: cell_edge) {
        ret += winding_number(V,edge(ce),p);
    }
    return ret;
}
template <typename Derived>
std::map<int,typename Derived::Scalar> HalfEdgeMesh::signed_areas(const Eigen::MatrixBase<Derived>& V) const {
    auto cem = cell_halfedges_multi_component_map();
    std::map<int,typename Derived::Scalar> ret;
    for(auto&& [c,hes]: cem) {
        ret[c] = signed_area(V,hes);
    }
    return ret;
}
template <typename Derived>
auto HalfEdgeMesh::signed_area(const Eigen::MatrixBase<Derived>& V, const HalfEdge& cell_edge) const -> typename Derived::Scalar {
    using S = typename Derived::Scalar;

    S ret = 0;
    auto edge = [](HalfEdge he) -> std::array<int,2> {
        int i = he.vertex(); he.next(); 
        int j = he.vertex();
        return {{i,j}};
    };

    get_cell_iterator(cell_edge)([&](auto&& e){
            auto [i,j] = edge(e);
            auto a = V.col(i);
            auto b = V.col(j);
            ret += a.x() * b.y() - a.y() * b.x();
    });
    return ret;
}
template <typename Derived>
auto HalfEdgeMesh::signed_area(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_edge) const -> typename Derived::Scalar {
    using S = typename Derived::Scalar;

    S ret = 0;
    for(auto&& ce: cell_edge) {
        ret += signed_area(V,edge(ce));
    }
    return ret;
}

template <typename Derived>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_edge, const std::vector<int>& vertex_indices) const {
    for(auto&& vi: vertex_indices) {
        if(!is_inside(V,cell_edge,V.col(vi))) {
            return false;
        }
    }
    return true;
}

template <typename Derived, typename Derived2>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_edge, const Eigen::MatrixBase<Derived2>& p) const {
    if constexpr(Derived2::ColsAtCompileTime == 1) {
        return std::abs(HalfEdgeMesh::winding_number(V,cell_edge,p)) > 1;//1 is ok, looking for multiples of 2pi right?
    } else {
        for(int i = 0; i < p.cols(); ++i) {
            if(!is_inside(V,cell_edge,p.col(i))) {
                return false;
            }
        }
        return true;
    }
}
template <typename Derived>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V, const HalfEdge& cell_edge, const std::vector<int>& vertex_indices) const {
    for(auto&& vi: vertex_indices) {
        if(!is_inside(V,cell_edge,V.col(vi))) {
            return false;
        }
    }
    return true;
}

template <typename Derived, typename Derived2>
bool HalfEdgeMesh::is_inside(const Eigen::MatrixBase<Derived>& V, const HalfEdge& cell_edge, const Eigen::MatrixBase<Derived2>& p) const {
    if constexpr(Derived2::ColsAtCompileTime == 1) {
        return std::abs(HalfEdgeMesh::winding_number(V,cell_edge,p)) > 1;//1 is ok, looking for multiples of 2pi right?
    } else {
        for(int i = 0; i < p.cols(); ++i) {
            if(!is_inside(V,cell_edge,p.col(i))) {
                return false;
            }
        }
        return true;
    }
}
template <typename S, int D>
template <typename Derived>
bool EmbeddedHalfEdgeMesh<S,D>::is_inside(const std::set<int>& cell_edge, const Eigen::MatrixBase<Derived>& p) const {
    return HalfEdgeMesh::is_inside(m_vertices,cell_edge,p);
}
template <typename S, int D>
bool EmbeddedHalfEdgeMesh<S,D>::is_inside(const std::set<int>& cell_edge, const std::vector<int>& vertex_indices) const {
    return HalfEdgeMesh::is_inside(m_vertices,cell_edge,vertex_indices);
}

template <typename S, int D>
template <typename Derived>
bool EmbeddedHalfEdgeMesh<S,D>::is_inside(int cell_edge, const Eigen::MatrixBase<Derived>& p) const {
    return is_inside(edge(cell_edge),p);
}
template <typename S, int D>
template <typename Derived>
bool EmbeddedHalfEdgeMesh<S,D>::is_inside(const HalfEdge& cell_edge, const Eigen::MatrixBase<Derived>& p) const {
    return HalfEdgeMesh::is_inside(m_vertices, cell_edge, p);
}
template <typename S, int D>
bool EmbeddedHalfEdgeMesh<S,D>::is_inside(int cell_edge, const std::vector<int>& vertex_indices) const {
    return is_inside(edge(cell_edge),vertex_indices);
}
template <typename S, int D>
bool EmbeddedHalfEdgeMesh<S,D>::is_inside(const HalfEdge& cell_edge, const std::vector<int>& vertex_indices) const {
    return HalfEdgeMesh::is_inside(m_vertices, cell_edge, vertex_indices);
}
template <typename S, int D>
mtao::ColVectors<int,3> EmbeddedHalfEdgeMesh<S,D>::cells_triangulated() const {
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
    return mtao::geometry::mesh::earclipping(m_vertices,cells());
}
template <typename S, int D>
template <typename Derived>
int EmbeddedHalfEdgeMesh<S,D>::get_cell(const Eigen::MatrixBase<Derived>& p) const {
    static_assert(D == 2);
    auto C = cell_halfedges_multi_component_map();


    for(auto&& [c, hes]: C) {
        if(is_inside(hes,p)) {
            return c;
        }
    }
    return -1;
}
template <typename S, int D>
mtao::VectorX<int> EmbeddedHalfEdgeMesh<S,D>::get_cells(const mtao::ColVectors<S,D>& P) const {
    auto C = cell_halfedges();
    mtao::VectorX<int> indices(P.cols());

    auto edge = [](HalfEdge he) -> std::array<int,3> {
        int i = he.vertex(); he.next(); 
        int j = he.vertex(); he.next(); 
        int k = he.vertex(); he.next(); 
        return {{i,j,k}};

    };

    auto get_index = [&](auto&& p) -> int {
        for(size_t i = 0; i < C.size(); ++i) {
            bool inside = true;
            cell_iterator(this,C[i]).run_earlyout([&](auto&& e) -> bool{
                    auto [i,j,k] = edge(e);
                    auto a = V(i);
                    auto b = V(j);
                    auto c = V(k);
                    auto ba = b-a;
                    auto cb = c-b;
                    auto pb = p-b;
                    Vec n(-ba.y(),ba.x());
                    inside = pb.dot(n) * cb.dot(n) >= 0;

                    return inside;
                    });
            if(inside) {
                return i;
            }
        }
        return -1;
    };

    for(int k = 0; k < P.cols(); ++k) {
        auto& idx = indices(k);
        auto p = P.col(k);
        idx = get_index(p);

    }
    return indices;
}

template <typename S, int D>
std::set<std::tuple<S,int>> EmbeddedHalfEdgeMesh<S,D>::get_edge_angles(int eidx) const {
    std::set<std::tuple<S,int>> edge_angles;

    vertex_iterator(this,eidx)([&](auto&& e) {
            auto t = T(e.index());
            S ang = std::atan2(t.y(),t.x());
            edge_angles.emplace({ang,e.index()});
            });
    /*
       std::transform(edges.begin(),edges.end(),std::inserter(edge_angles,edge_angles.end()), [&](int eidx) {
       HalfEdge e = edge(eidx);
       auto p = V(e.get_dual().vertex()) - o;
       T ang = std::atan2(p.y(),p.x());
       return std::make_pair(ang,eidx);
       });
       */
    return edge_angles;
}


template <typename Derived>
void HalfEdgeMesh::tie_nonsimple_cells(const Eigen::MatrixBase<Derived>& V, const std::set<int>& cell_halfedges) {
    auto vols = signed_areas(V);
    std::map<int,std::set<int>> halfedge_partial_ordering;
    auto cells_map = this->cells_map();

    std::map<int,std::vector<int>> CM;
    for(auto&& he: cell_halfedges) {
        CM[he] = cell_he(he);
    }

    mtao::data_structures::DisjointSet<int> ds;
    ds.add_node(-1);
    for(auto&& he: cell_halfedges) {
        int cell = cell_index(he);
        ds.add_node(cell);
        ds.add_node(dual_index(cell));
        if(vols[cell] <= 0) {
            continue;
        }

        for(auto&& he_m: cell_halfedges) {
            int c = cell_index(he_m);
            if(c == cell || vols[c] <= 0) {
                continue;
            } else {
                if( is_inside(V,edge(he),CM[he_m])) {
                    halfedge_partial_ordering[he].insert(he_m);
                }
            }

        }

    }
    std::map<int,std::set<int>> ordered_partial_ordering;
    std::set<int> seen_cells;
    for(auto&& [h, hs]: halfedge_partial_ordering) {
        ordered_partial_ordering[hs.size()].insert(h);
    }
    for(auto&& pr: ordered_partial_ordering) {
        auto&& hs = std::get<1>(pr);
        for(auto&& h: hs) {
            int cell = cell_index(h);
            auto&& children = halfedge_partial_ordering[h];

            for(auto&& c: children) {
                if(seen_cells.find(c) == seen_cells.end()) {
                    int d = dual_index(c);
                    if(d == -1) continue;
                    int cell2 = cell_index(d);
                    ds.join(cell,cell2);
                    seen_cells.emplace(c);
                }
            }
        }
    }
    ds.reduce_all();
    {

        auto ci = cell_indices();
        for(int i = 0; i < size(); ++i) {
            int c = ci(i);
            if(ds.has_node(c)) {
                int root = ds.get_root(ci(i)).data;
                cell_indices()(i) = root;
            }
        }
    }
}
}}}
#endif//HALFEDGE_CELLCOMPLEX_H
