#include "mtao/opengl/objects/mesh.h"
#include "mtao/geometry/mesh/vertex_normals.hpp"
#include <Magnum/Shaders/Phong.h>
#include <Corrade/Containers/Array.h>
#include <Magnum/MeshTools/CompressIndices.h>


using namespace Magnum;
using namespace Math::Literals;
namespace mtao::opengl::objects {
    
    Mesh::Mesh(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 3>& F) {
        setData(V,F);
    }
    void Mesh::setData(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 3>& F) {

        set_index_buffer(F);
        vertex_buffer.setData(Containers::ArrayView<const float>{V.data(),V.size()});
        
        auto N = mtao::geometry::mesh::vertex_normals(V,F);
        normal_buffer.setData(Containers::ArrayView<const float>{N.data(),N.size()});
    setPrimitive(GL::MeshPrimitive::Triangles)
        .setCount(F.size())
        .addVertexBuffer(vertex_buffer, 0,
                Shaders::Phong::Position{})
        .addVertexBuffer(normal_buffer, 0, Shaders::Phong::Normal{})
        .setIndexBuffer(index_buffer,0,indexType,indexStart,indexEnd);

        //mtao::ColVectors<float,6> VN(6,V.cols());
        //VN.topRows<3>() = V;
        //VN.bottomRows<3>() = mtao::geometry::mesh::vertex_normals(V,F);
        //vbuffer.setData(Containers::ArrayView<const float>{VN.data(),VN.size()});
    }



    void Mesh::set_index_buffer(const mtao::ColVectors<unsigned int, 3>& F) {
        Containers::Array<char> indexData;
        std::vector<unsigned int> inds(F.data(),F.data()+F.size());

        std::tie(indexData, indexType, indexStart, indexEnd) =
            MeshTools::compressIndices(inds);
        index_buffer.setData(indexData);
    }
}
