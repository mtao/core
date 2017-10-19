#include "geometry/mesh/halfedge_cellcomplex.h"
#incldue <map>
#include <tuple>

using HalfEdge = HalfEdgeCellComplex::HalfEdge;

HalfEdgeCellComplex::HalfEdgeCellComplex(const std::string& str) {
}

HalfEdgeCellComplex::HalfEdgeCellComplex(const Mat3Xf& V, const Eigen::MatrixXi& F): m_vertices(V) {
    using Edge = std::tuple<int,int>;

    auto cell_size = [](auto vec) {
        for(int j = 0; j < f.rows(); ++j) {
            if(f(j) == -1) {
                return ;
            }
        }
    };

    int size = 0;

    for(int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        size += cell_size(f);
    }

    m_edges.resize(4,size);

    std::map<Edge,int> edge_indices;

    int counter = 0;
    for(int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        int size = cell_size(f);
        int counter_start = counter;
        for(int j = 0; j < size-1; ++j) {
            m_edges.col(counter) = Eigen::Vector4i(f(j),i,counter+1,-1);
            edge_indices[{f(j),f(j+1)}] = counter++;
        }
        m_edges.col(counter) = Eigen::Vector4i(f(j),i,counter_start,-1);
        edge_indices[{f(j),f(0)}] = counter++;
    }

    for(int i = 0; i < m_edges.size(); ++i) {
        auto it = edge_indices.find({edge_vertex(edge_next(i)),edge_vertex(i)});
        if(it != edge_indices.end()) {
            edge_duals()(i) = *it;
        }
    }

}

HalfEdge::HalfEdge(const MeshType* cc, int idx): m_cc(cc), m_index(idx) {}

HalfEdge HalfEdge::next() const {
    m_index = next_index();
    return *this;
}
HalfEdge HalfEdge::dual() const {
    m_index = dual_index();
    return *this;
}
int HalfEdge::next_index() const {
    return m_cc->edge_next(index());
}
int HalfEdge::next_dual() const {
    return m_cc->edge_dual(index());
}
HalfEdge HalfEdge::next_index() const {
    return HalfEdge(m_cc,m_cc->edge_next(index()));
}
HalfEdge HalfEdge::next_dual() const {
    return HalfEdge(m_cc,m_cc->edge_dual(index()));
}
