#include "mtao/opengl/objects/mesh.h"
#include "mtao/geometry/mesh/vertex_normals.hpp"
#include <Magnum/Shaders/Phong.h>
#include <Corrade/Containers/Array.h>
#include <Magnum/MeshTools/CompressIndices.h>


using namespace Magnum;
using namespace Math::Literals;
namespace mtao::opengl::objects {
    Mesh::Mesh(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 2>& F) {
        setEdgeData(V,F);
    }
    void Mesh::setEdgeData(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 2>& E) {

        set_edge_index_buffer(E);
        vertex_buffer.setData(Containers::ArrayView<const float>{V.data(),size_t(V.size())});
    
    }
    Mesh::Mesh(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 3>& F) {
        setTriangleData(V,F);
    }
    void Mesh::setTriangleData(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 3>& F) {

        set_triangle_index_buffer(F);
        vertex_buffer.setData(Containers::ArrayView<const float>{V.data(),size_t(V.size())});
        
        auto N = mtao::geometry::mesh::vertex_normals(V,F);
        normal_buffer.setData(Containers::ArrayView<const float>{N.data(),size_t(N.size())});
        setCount(F.size())
        .addVertexBuffer(vertex_buffer, 0,
                Shaders::Phong::Position{})
        .addVertexBuffer(normal_buffer, 0, Shaders::Phong::Normal{})
        .setIndexBuffer(triangle_index_buffer,0,triangle_indexType,triangle_indexStart,triangle_indexEnd);

        //mtao::ColVectors<float,6> VN(6,V.cols());
        //VN.topRows<3>() = V;
        //VN.bottomRows<3>() = mtao::geometry::mesh::vertex_normals(V,F);
        //vbuffer.setData(Containers::ArrayView<const float>{VN.data(),VN.size()});
    }


    void Mesh::set_edge_index_buffer(const mtao::ColVectors<unsigned int, 2>& E) {
        setPrimitive(GL::MeshPrimitive::Lines);
        Containers::Array<char> indexData;
        std::vector<unsigned int> inds(E.data(),E.data()+E.size());

        std::tie(indexData, edge_indexType, edge_indexStart, edge_indexEnd) =
            MeshTools::compressIndices(inds);
        edge_index_buffer.setData(indexData);
    }

    void Mesh::set_triangle_index_buffer(const mtao::ColVectors<unsigned int, 3>& F) {
        setPrimitive(GL::MeshPrimitive::Triangles);
        Containers::Array<char> indexData;
        std::vector<unsigned int> inds(F.data(),F.data()+F.size());

        std::tie(indexData, triangle_indexType, triangle_indexStart, triangle_indexEnd) =
            MeshTools::compressIndices(inds);
        triangle_index_buffer.setData(indexData);
    }
}
