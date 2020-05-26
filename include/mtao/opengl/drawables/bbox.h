#pragma once
#pragma once
#include "mtao/opengl/drawables.h"
#include "mtao/opengl/objects/bbox.h"

namespace mtao::opengl::drawables {
    template <int D, typename ShaderType = std::conditional_t<D==2, Magnum::Shaders::Flat2D, Magnum::Shaders::Flat3D>>
    class BoundingBox: public objects::BoundingBox<D>, public Drawable<ShaderType> {
        public:
        using objects::Mesh<D>::setVertexBuffer;
        using objects::Mesh<D>::setNormalBuffer;
        using objects::Mesh<D>::setColorBuffer;
        using objects::Mesh<D>::setEdgeBuffer;
        using objects::Mesh<D>::setTriangleBuffer;
        using Drawable<ShaderType>::activate_points;
        using Drawable<ShaderType>::activate_edges;
        using Drawable<ShaderType>::activate_triangles;


        using DrawableGroup = typename Drawable<ShaderType>::DrawableGroup;


        template <typename... Args>
            BoundingBox(ShaderType& shader, DrawableGroup& group, Args&&... args): objects::BoundingBox<D>{std::forward<Args>(args)...}, Drawable<ShaderType>{*this, shader, group}
        {
            Drawable<ShaderType>::activate_points({});
            Drawable<ShaderType>::activate_triangles({});
            Drawable<ShaderType>::activate_edges();
        }

    };
}
