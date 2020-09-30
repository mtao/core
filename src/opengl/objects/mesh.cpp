#include "mtao/opengl/objects/mesh.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/Shaders/Phong.h>

#include <numeric>

#include "mtao/geometry/mesh/vertex_normals.hpp"

using namespace Magnum;
using namespace Math::Literals;
namespace mtao::opengl::objects {
AlgebraicMesh::AlgebraicMesh(const mtao::ColVectors<unsigned int, 2>& E) {
    setEdgeBuffer(E);
}
AlgebraicMesh::AlgebraicMesh(const mtao::ColVectors<unsigned int, 3>& F) {
    setTriangleBuffer(F);
}
void AlgebraicMesh::makeVertexIndexBuffer(unsigned int size) {
    vertex_Count = size;
    std::vector<unsigned int> inds(size);
    Containers::Array<char> indexData;
    std::iota(inds.begin(), inds.end(), (unsigned int)(0));
    std::tie(indexData, vertex_indexType) = MeshTools::compressIndices(
        Corrade::Containers::StridedArrayView1D<unsigned int>(inds));
    vertex_index_buffer.setData(indexData);
}
void AlgebraicMesh::setEdgeBuffer(const mtao::ColVectors<unsigned int, 2>& E) {
    setPrimitive(GL::MeshPrimitive::Lines);
    Containers::Array<char> indexData;
    std::vector<unsigned int> inds(E.data(), E.data() + E.size());

    edge_Count = E.size();

    std::tie(indexData, edge_indexType) = MeshTools::compressIndices(
        Corrade::Containers::StridedArrayView1D<unsigned int>(inds));
    edge_index_buffer.setData(indexData);
}
void AlgebraicMesh::setTriangleBuffer(
    const mtao::ColVectors<unsigned int, 3>& F) {
    setPrimitive(GL::MeshPrimitive::Triangles);
    Containers::Array<char> indexData;
    std::vector<unsigned int> inds(F.data(), F.data() + F.size());
    triangle_Count = F.size();

    std::tie(indexData, triangle_indexType) = MeshTools::compressIndices(
        Corrade::Containers::StridedArrayView1D<unsigned int>(inds));

    triangle_index_buffer.setData(indexData);
}
template <>
void Mesh<3>::setTriangleBuffer(const ColVecs& V,
                                const mtao::ColVectors<unsigned int, 3>& F) {
    setVertexBuffer(V);
    AlgebraicMesh::setTriangleBuffer(F);
    auto N = mtao::geometry::mesh::vertex_normals(V, F);
    normal_buffer.setData(
        Containers::ArrayView<const float>{N.data(), size_t(N.size())});
}

}  // namespace mtao::opengl::objects
