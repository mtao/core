#ifndef _TRIFORCE_MESH_GEN_H_
#define _TRIFORCE_MESH_GEN_H_
#include <array>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <Eigen/Dense>
#include "compat.h"
template<typename Scalar>
class TriforceMeshFactory {
  public:
    typedef mtao::compat::array<int, 3> Face;
    typedef mtao::compat::array<int, 2> Edge;
    typedef Scalar Scalar;
    typedef typename Eigen::Matrix<Scalar, 3, 1> Vector;
    using VecVector = mtao::vector<Vector>;
    TriforceMeshFactory(int depth = 3);
    void triforce(const Face &f, int depth);
    int add_edge(Edge e);
    void write(const std::string &filename);
    void write(std::ostream &outstream);
    const std::vector<Face> faces() const { return m_faces; }
    const VecVector vertices() const { return m_vertices; }

  private:
    const int m_depth = 0;
    VecVector m_vertices;
    std::vector<Face> m_faces;
    mtao::IndexMap<2> m_edges;
    //std::map<Edge,  int> m_edges;
};


template<typename T>
TriforceMeshFactory<T>::TriforceMeshFactory(int depth) : m_depth(depth) {
}
template<typename T>
void TriforceMeshFactory<T>::run_in_place(VecVector &vertices, std::vector<Face> &faces) {
    //Create icosahedron base

    Scalar gr = .5 * (1 + std::sqrt(Scalar(5)));
    m_vertices.resize(5);

    m_vertices[0] = Vector(0, -1, 0);
    m_vertices[1] = Vector(0, 1, 0);
    m_vertices[2] = Vector(-1, 0, 0);
    m_vertices[3] = Vector(1, 0, 0);
    m_vertices[4] = Vector(0, 0, 1);

    triforce({ { 0, 1, 4 } }, depth);
    triforce({ { 1, 2, 4 } }, depth);
    triforce({ { 2, 3, 4 } }, depth);
    triforce({ { 3, 0, 4 } }, depth);
}
template<typename T>
void TriforceMeshFactory<T>::triforce(const Face &f, int depth) {
    if (depth <= 0) {
        m_faces.push_back(f);
    } else {
        int e01 = add_edge({ { f[0], f[1] } });
        int e12 = add_edge({ { f[1], f[2] } });
        int e02 = add_edge({ { f[0], f[2] } });
        triforce({ { f[0], e01, e02 } }, depth - 1);
        triforce({ { f[1], e12, e01 } }, depth - 1);
        triforce({ { f[2], e02, e12 } }, depth - 1);
        triforce({ { e01, e12, e02 } }, depth - 1);
    }
}

template<typename T>
int TriforceMeshFactory<T>::add_edge(Edge e) {
    if (e[0] > e[1]) {
        int tmp = e[0];
        e[0] = e[1];
        e[1] = tmp;
    }
    auto it = m_edges.find(e);
    if (it != m_edges.end()) {
        return it->second;
    } else {
        m_edges[e] = m_vertices.size();
        m_vertices.push_back(
          (m_vertices[e[0]] + m_vertices[e[1]]).normalized());
        return m_vertices.size() - 1;
    }
}


template<typename T>
void TriforceMeshFactory<T>::write(const std::string &filename) {
    std::ofstream outstream(filename.c_str());
    write(outstream);
}

template<typename T>
void TriforceMeshFactory<T>::write(std::ostream &outstream) {
    outstream << "#Icosahedral subdivision to depth " << m_depth << std::endl;
    for (auto &&v : m_vertices) {
        outstream << "v " << v.transpose() << std::endl;
    }

    for (auto &&f : m_faces) {
        outstream << "f " << f[0] + 1 << " " << f[1] + 1 << " " << f[2] + 1 << std::endl;
    }
}
#endif
