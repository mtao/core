#pragma once
#include "mtao/opengl/drawables.h"
#include "mtao/opengl/objects/mesh.h"

namespace mtao::opengl::drawables {


    template <typename ShaderType, int D = internal::ShaderDim<ShaderType>()>
    class Mesh: public objects::Mesh<D>, public Drawable<ShaderType> {
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
            Mesh(ShaderType& shader, DrawableGroup& group, Args&&... args): objects::Mesh<D>{std::forward<Args>(args)...}, Drawable<ShaderType>{*this, shader, group}
        {}

        //template <typename ShaderType_>
        //Mesh(ShaderType& shader, Mesh<ShaderType_, D>&& other): Mesh{shader, *other.Drawable<ShaderType_>::group(), std::move(other) {
        //    Drawable<ShaderType>::group()->remove(other);
        //}
        //template <typename ShaderType_>
        //Mesh operator=(Mesh<ShaderType_, D>&& other) {
        //Mesh{shader, *other.Drawable<ShaderType_>::group(), std::move(other) {

        //    Drawable<ShaderType>::group()->remove(other);
        //}


    };


};
