#pragma once
#include "mtao/types.hpp"
#include "types.h"

#include <Magnum/GL/Buffer.h>

namespace mtao::opengl::objects {
    using namespace Magnum;

    class AlgebraicMesh: public GL::Mesh {
        public:
            AlgebraicMesh() {}
            AlgebraicMesh(const mtao::ColVectors<unsigned int, 3>& F);
            AlgebraicMesh(const mtao::ColVectors<unsigned int, 2>& E);
            void setTriangleBuffer(const mtao::ColVectors<unsigned int, 3>& F);
            void setEdgeBuffer(const mtao::ColVectors<unsigned int, 2>& E);

            template <typename Derived>
                void setVertexBuffer(const Eigen::PlainObjectBase<Derived>& V) {
                    vertex_buffer.setData(Containers::ArrayView<const float>{V.data(),size_t(V.size())});
                }
            template <typename Derived>
                void setColorBuffer(const Eigen::PlainObjectBase<Derived>& V) {
                    color_buffer.setData(Containers::ArrayView<const float>{V.data(),size_t(V.size())});
                }


            Magnum::GL::Buffer vertex_buffer, edge_index_buffer, triangle_index_buffer, normal_buffer, vfield_buffer, color_buffer;

            Magnum::UnsignedInt edge_Count;
            Magnum::MeshIndexType edge_indexType;
            Magnum::UnsignedInt edge_indexStart, edge_indexEnd;

            Magnum::UnsignedInt triangle_Count;
            Magnum::MeshIndexType triangle_indexType;
            Magnum::UnsignedInt triangle_indexStart, triangle_indexEnd;
    };

    template <int D>
        class MeshObject;
    template <>
    class MeshObject<2>: public Object2D, public AlgebraicMesh {};
    template <>
    class MeshObject<3>: public Object3D, public AlgebraicMesh {};


    template <int D>
    class Mesh: public MeshObject<D> {
        public:

            using AlgebraicMesh::setTriangleBuffer;
            using AlgebraicMesh::setEdgeBuffer;
            using AlgebraicMesh::setVertexBuffer;
            using ColVecs = mtao::ColVectors<float,D>;
            Mesh() {}
            Mesh(const ColVecs& V, const mtao::ColVectors<unsigned int, 3>& F) {
                setTriangleBuffer(V,F);
            }
            Mesh(const ColVecs& V, const mtao::ColVectors<unsigned int, 2>& E) {
                setEdgeBuffer(V,E);
            }
            void setTriangleBuffer(const ColVecs& V, const mtao::ColVectors<unsigned int, 3>& F) {
                setVertexBuffer(V);
                setTriangleBuffer(F);
            }
            void setEdgeBuffer(const ColVecs& V, const mtao::ColVectors<unsigned int, 2>& E) {
                setVertexBuffer(V);
                setEdgeBuffer(E);
            }
    };

    template <>
        void Mesh<3>::setTriangleBuffer(const ColVecs& V, const mtao::ColVectors<unsigned int, 3>& F);



}
