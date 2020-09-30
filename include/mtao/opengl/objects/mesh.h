#pragma once
#include <Magnum/GL/Buffer.h>

#include "mtao/types.hpp"
#include "types.h"

namespace mtao::opengl::objects {
using namespace Magnum;

class AlgebraicMesh : public GL::Mesh {
   public:
    AlgebraicMesh() {}
    AlgebraicMesh(const mtao::ColVectors<unsigned int, 3>& F);
    AlgebraicMesh(const mtao::ColVectors<unsigned int, 2>& E);
    void makeVertexIndexBuffer(unsigned int size);
    void setTriangleBuffer(const mtao::ColVectors<unsigned int, 3>& F);
    void setEdgeBuffer(const mtao::ColVectors<unsigned int, 2>& E);

    template <typename Derived>
    void setVertexBuffer(const Eigen::PlainObjectBase<Derived>& V) {
        makeVertexIndexBuffer(V.cols());
        vertex_buffer.setData(
            Containers::ArrayView<const float>{V.data(), size_t(V.size())});
    }
    void setVertexBuffer(const Containers::ArrayView<const float>& V,
                         int count = 1) {
        makeVertexIndexBuffer(count);
        vertex_buffer.setData(V);
    }
    template <typename Derived>
    void setColorBuffer(const Eigen::PlainObjectBase<Derived>& V) {
        color_buffer.setData(
            Containers::ArrayView<const float>{V.data(), size_t(V.size())});
    }
    template <typename Derived>
    void setVFieldBuffer(const Eigen::PlainObjectBase<Derived>& V) {
        vfield_buffer.setData(
            Containers::ArrayView<const float>{V.data(), size_t(V.size())});
    }
    template <typename Derived>
    void setNormalBuffer(const Eigen::PlainObjectBase<Derived>& V) {
        normal_buffer.setData(
            Containers::ArrayView<const float>{V.data(), size_t(V.size())});
    }

    Magnum::GL::Buffer vertex_buffer, vertex_index_buffer, edge_index_buffer,
        triangle_index_buffer, normal_buffer, vfield_buffer, color_buffer;
    Magnum::Int vertex_Count;
    Magnum::MeshIndexType vertex_indexType;

    Magnum::UnsignedInt edge_Count;
    Magnum::MeshIndexType edge_indexType;

    Magnum::UnsignedInt triangle_Count;
    Magnum::MeshIndexType triangle_indexType;
};

template <int D>
class MeshObject;
template <>
class MeshObject<2> : public Object2D, public AlgebraicMesh {};
template <>
class MeshObject<3> : public Object3D, public AlgebraicMesh {};

template <int D>
class Mesh : public MeshObject<D> {
   public:
    using AlgebraicMesh::setEdgeBuffer;
    using AlgebraicMesh::setTriangleBuffer;
    using AlgebraicMesh::setVertexBuffer;
    using ColVecs = mtao::ColVectors<float, D>;
    Mesh() {}
    Mesh(const ColVecs& V, const mtao::ColVectors<unsigned int, 3>& F) {
        setTriangleBuffer(V, F);
    }
    Mesh(const ColVecs& V, const mtao::ColVectors<unsigned int, 2>& E) {
        setEdgeBuffer(V, E);
    }
    void setVertexBuffer(const Containers::ArrayView<const float>& V) {
        setVertexBuffer(V, V.size() / D);
    }
    template <typename T>
    void setVertexBuffer(const Math::Vector<D, T>& V) {
        setVertexBuffer(Containers::ArrayView<const float>(V.data(), D));
    }
    void setTriangleBuffer(const ColVecs& V,
                           const mtao::ColVectors<unsigned int, 3>& F) {
        setVertexBuffer(V);
        setTriangleBuffer(F);
    }
    void setEdgeBuffer(const ColVecs& V,
                       const mtao::ColVectors<unsigned int, 2>& E) {
        setVertexBuffer(V);
        setEdgeBuffer(E);
    }
};

template <>
void Mesh<3>::setTriangleBuffer(const ColVecs& V,
                                const mtao::ColVectors<unsigned int, 3>& F);

template <int D>
class EdgeMesh : public Mesh<D> {};

}  // namespace mtao::opengl::objects
