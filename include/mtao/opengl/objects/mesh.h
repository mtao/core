#pragma once
#include "mtao/types.hpp"
#include "types.h"

#include <Magnum/GL/Buffer.h>

namespace mtao::opengl::objects {
    using namespace Magnum;

    class MeshObject2: public Object2D, public GL::Mesh {};
    class MeshObject3: public Object3D, public GL::Mesh {};

    class Mesh: public MeshObject3 {
        public:
            Mesh() {}
            Mesh(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 3>& F);
            Mesh(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 2>& E);
            void setTriangleData(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 3>& F);
            void setEdgeData(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 2>& E);

            void set_edge_index_buffer(const mtao::ColVectors<unsigned int, 2>& E) ;
            void set_triangle_index_buffer(const mtao::ColVectors<unsigned int, 3>& F) ;

            Magnum::GL::Buffer vertex_buffer, edge_index_buffer, triangle_index_buffer, normal_buffer, vfield_buffer, color_buffer;

            Magnum::MeshIndexType edge_indexType;
            Magnum::UnsignedInt edge_indexStart, edge_indexEnd;

            Magnum::MeshIndexType triangle_indexType;
            Magnum::UnsignedInt triangle_indexStart, triangle_indexEnd;
    };



}
