#pragma once
#include "mtao/geometry/mesh/halfedge.hpp"

namespace mtao::geometry::mesh {
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
}  // namespace mtao::geometry::mesh
