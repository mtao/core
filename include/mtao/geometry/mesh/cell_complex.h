
#ifndef CELLCOMPLEX_H
#define CELLCOMPLEX_H
#include <Eigen/Dense>
#include <vector>


class CellComplex {
  public:
    using Mat3X = Eigen::Matrix<float, 3, Eigen::Dynamic>;
    using VecI = Eigen::Matrix<int, Eigen::Dynamic, 1>;
    using Edges = Eigen::Matrix<int, Eigen::Dynamic, 2>;


    auto edge_vertices() const { return m_edges.col<0>(); }
    auto edge_duals() const { return m_edges.col<1>(); }
    int edge_vertex(int idx) const { return edge_vertices(idx); }
    int edge_dual(int idx) const { return edge_duals(idx); }

  private:
    Mat3X m_vertices;
    VecI m_cells;
    Edges m_edges;
};


#endif//CELLCOMPLEX_H
