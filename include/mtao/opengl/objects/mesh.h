#pragma once
#include "mtao/types.hpp"
#include "types.h"

#include <Magnum/GL/Buffer.h>

namespace mtao::opengl::objects {

    class Mesh: public MeshObject3 {
        public:
            Mesh() {}
            Mesh(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 3>& F);
            void setData(const mtao::ColVecs3f& V, const mtao::ColVectors<unsigned int, 3>& F);

            void set_index_buffer(const mtao::ColVectors<unsigned int, 3>& F) ;

        private:
            Magnum::GL::Buffer vertex_buffer, index_buffer, normal_buffer, vfield_buffer, color_buffer;

            Magnum::MeshIndexType indexType;
            Magnum::UnsignedInt indexStart, indexEnd;
    };



}
