#ifndef _SPHERE_MESH_GEN_IMPL_H_
#define _SPHERE_MESH_GEN_IMPL_H_

#include "mtao/types.h"
#include <tuple>
#include <array>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <Eigen/Dense>

namespace mtao { namespace geometry { namespace mesh {
    namespace detail {
        template <typename Scalar_>
            class SphereMeshFactory{
                typedef std::array<unsigned int, 3> Face;
                typedef std::array<unsigned int, 2> Edge;
                public:
                typedef Scalar_ Scalar;
                typedef typename mtao::Vector<Scalar,3> Vector;
                SphereMeshFactory(int depth=3);
                void triforce(const Face & f, int depth);
                unsigned int add_edge(Edge e);
                void write(const std::string & filename);
                void write(std::ostream & outstream);
                const std::vector<Face> faces() const {return m_faces;}
                const std::vector<Vector> vertices() const {return m_vertices;}

            mtao::ColVectors<Scalar,3> V() const ;
            mtao::ColVectors<int,3> F() const ;
                private:
                const int m_depth = 0;
                mtao::vector<Vector> m_vertices;
                mtao::vector<Face> m_faces;
                std::map<Edge, unsigned int> m_edges;

            };



        template <typename T>
            SphereMeshFactory<T>::SphereMeshFactory(int depth): m_depth(depth) {
                //Create icosahedron base

                Scalar gr = .5 * (1 + std::sqrt(Scalar(5)));
                m_vertices.resize(12);

                m_vertices[ 0] = Vector(     0,    - 1,     gr);
                m_vertices[ 1] = Vector(    gr,      0,      1);
                m_vertices[ 2] = Vector(    gr,      0,    - 1);
                m_vertices[ 3] = Vector(   -gr,      0,    - 1);
                m_vertices[ 4] = Vector(   -gr,      0,      1);
                m_vertices[ 5] = Vector(   - 1,     gr,      0);
                m_vertices[ 6] = Vector(     1,     gr,      0);
                m_vertices[ 7] = Vector(     1,    -gr,      0);
                m_vertices[ 8] = Vector(   - 1,    -gr,      0);
                m_vertices[ 9] = Vector(     0,    - 1,    -gr);
                m_vertices[10] = Vector(     0,      1,    -gr);
                m_vertices[11] = Vector(     0,      1,     gr);
                for(auto&& v: m_vertices) {
                    v.normalize();
                }

                triforce({{ 1 ,  2 ,  6}},depth); 
                triforce({{ 1 ,  7 ,  2}},depth); 
                triforce({{ 3 ,  4 ,  5}},depth); 
                triforce({{ 4 ,  3 ,  8}},depth); 
                triforce({{ 6 ,  5 , 11}},depth); 
                triforce({{ 5 ,  6 , 10}},depth); 
                triforce({{ 9 , 10 ,  2}},depth); 
                triforce({{10 ,  9 ,  3}},depth); 
                triforce({{ 7 ,  8 ,  9}},depth); 
                triforce({{ 8 ,  7 ,  0}},depth); 
                triforce({{11 ,  0 ,  1}},depth); 
                triforce({{ 0 , 11 ,  4}},depth); 
                triforce({{ 6 ,  2 , 10}},depth); 
                triforce({{ 1 ,  6 , 11}},depth); 
                triforce({{ 3 ,  5 , 10}},depth); 
                triforce({{ 5 ,  4 , 11}},depth); 
                triforce({{ 2 ,  7 ,  9}},depth); 
                triforce({{ 7 ,  1 ,  0}},depth); 
                triforce({{ 3 ,  9 ,  8}},depth); 
                triforce({{ 4 ,  8 ,  0}},depth); 



            }
        template <typename T>
            void SphereMeshFactory<T>::triforce(const Face & f, int depth) {
                if(depth <= 0) {
                    m_faces.push_back(f);
                } else {
                    unsigned int e01 = add_edge({{f[0],f[1]}});
                    unsigned int e12 = add_edge({{f[1],f[2]}});
                    unsigned int e02 = add_edge({{f[0],f[2]}});
                    triforce({{f[0],e01,e02}},depth-1);
                    triforce({{f[1],e12,e01}},depth-1);
                    triforce({{f[2],e02,e12}},depth-1);
                    triforce({{e01 ,e12,e02}},depth-1);
                }

            }

        template <typename T>
            unsigned int SphereMeshFactory<T>::add_edge(Edge e) {
                if(e[0] > e[1]) {
                    unsigned int tmp = e[0];
                    e[0] = e[1];
                    e[1] = tmp;
                }
                auto it = m_edges.find(e);
                if(it != m_edges.end()) {
                    return it->second;
                } else {
                    m_edges[e] = m_vertices.size();
                    m_vertices.push_back(
                            (m_vertices[e[0]] + m_vertices[e[1]]).normalized()
                            );
                    return m_vertices.size()-1;
                }


            }

        template <typename T>
            mtao::ColVectors<T,3> SphereMeshFactory<T>::V() const {
            mtao::ColVectors<T,3> v(3,m_vertices.size());
                for(size_t i = 0; i < m_vertices.size(); ++i) {
                    v.col(i) = m_vertices[i];
                }
                return v;
            }

        template <typename T>
            mtao::ColVectors<int,3> SphereMeshFactory<T>::F() const {
                mtao::ColVectors<int,3> f(3,m_faces.size());
                for(size_t i = 0; i < m_faces.size(); ++i) {
                    f.col(i) = Eigen::Map<const mtao::Vector<unsigned int,3>>(m_faces[i].data()).cast<int>();
                }
                return f;
            }

    }

}}}
#endif
