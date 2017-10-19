#ifndef HALFEDGE_CELLCOMPLEX_H
#define HALFEDGE_CELLCOMPLEX_H
#include <Eigen/Dense>
#include <vector>


class HalfEdgeCellComplex {
    public:
        using Mat3Xf = Eigen::Matrix<float,3, Eigen::Dynamic>;
        using VecXi = Eigen::Matrix<int,Eigen::Dynamic,1>;
        using Edges = Eigen::Matrix<int,4,Eigen::Dynamic>;

        enum class Index {VertexIndex,
            CellIndex,
            NextIndex,
            DualIndex};

        struct HalfEdge;

        HalfEdgeCellComplex(const std::string& str);
        HalfEdgeCellComplex(const Mat3Xf& V, const Eigen::MatrixXi& E);

        auto edge_vertices() { return m_edges.col<Index::VertexIndex>(); }
        auto edge_cells() { return m_edges.col<Index::CellIndex>(); }
        auto edge_nexts() { return m_edges.col<Index::NextIndex>(); }
        auto edge_duals() { return m_edges.col<Index::DualIndex>(); }
        auto edge_vertices() const { return m_edges.col<Index::VertexIndex>(); }
        auto edge_cells() const { return m_edges.col<Index::CellIndex>(); }
        auto edge_nexts() const { return m_edges.col<Index::NextIndex>(); }
        auto edge_duals() const { return m_edges.col<Index::DualIndex>(); }

        int edge_vertex(int idx) const { return edge_vertices()(idx); }
        int edge_dual(int idx) const { return edge_duals()(idx); }
        int edge_next(int idx) const { return edge_nexts()(idx); }
    private:
        Mat3Xf m_vertices;
        Edges m_edges;
        


};


struct HalfEdgeCellComplex::HalfEdge {
    public:
        using MeshType = HalfEdgeCellComplex;
        HalfEdge(const MeshType* cc, int idx);
        int index() const { return m_index; }
        int next_index() const;
        int next_dual() const;
        HalfEdge get_next() const;
        HalfEdge get_dual() const;
        HalfEdge& next();
        HalfEdge& dual();
    private:
        const MeshType* m_cc;
        int m_index = -1;
};

#endif//HALFEDGE_CELLCOMPLEX_H

