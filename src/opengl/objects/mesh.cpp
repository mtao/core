#include "mtao/opengl/objects/mesh.h"
#include "mtao/geometry/mesh/vertex_normals.hpp"
#include <Magnum/Shaders/Phong.h>
#include <Corrade/Containers/Array.h>
#include <Magnum/MeshTools/CompressIndices.h>


using namespace Magnum;
using namespace Math::Literals;
namespace mtao::opengl::objects {
    AlgebraicMesh::AlgebraicMesh(const mtao::ColVectors<unsigned int, 2>& E) {
        setEdgeBuffer(E);
    }
    AlgebraicMesh::AlgebraicMesh(const mtao::ColVectors<unsigned int, 3>& F) {
        setTriangleBuffer(F);
    }
    void AlgebraicMesh::setEdgeBuffer(const mtao::ColVectors<unsigned int, 2>& E) {
    
        setPrimitive(GL::MeshPrimitive::Lines);
        Containers::Array<char> indexData;
        std::vector<unsigned int> inds(E.data(),E.data()+E.size());

        edge_Count = E.cols();

        std::tie(indexData, edge_indexType, edge_indexStart, edge_indexEnd) =
            MeshTools::compressIndices(inds);
        edge_index_buffer.setData(indexData);
    }
    void AlgebraicMesh::setTriangleBuffer(const mtao::ColVectors<unsigned int, 3>& F) {

        setPrimitive(GL::MeshPrimitive::Triangles);
        Containers::Array<char> indexData;
        std::vector<unsigned int> inds(F.data(),F.data()+F.size());
        triangle_Count = F.cols();

        std::tie(indexData, triangle_indexType, triangle_indexStart, triangle_indexEnd) =
            MeshTools::compressIndices(inds);
        triangle_index_buffer.setData(indexData);
    }
    template <>
        void Mesh<3>::setTriangleBuffer(const ColVecs& V, const mtao::ColVectors<unsigned int, 3>& F) {
            setVertexBuffer(V);
            AlgebraicMesh::setTriangleBuffer(F);
            auto N = mtao::geometry::mesh::vertex_normals(V,F);
            normal_buffer.setData(Containers::ArrayView<const float>{N.data(),size_t(N.size())});
        }


}
