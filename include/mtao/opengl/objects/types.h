#pragma once
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/GL/Mesh.h>


namespace mtao::opengl {
        using MatTransform2D = Magnum::SceneGraph::MatrixTransformation2D;
        using Scene2D = Magnum::SceneGraph::Scene<MatTransform2D>;
        using Object2D = Magnum::SceneGraph::Object<MatTransform2D>;

        using MatTransform3D = Magnum::SceneGraph::MatrixTransformation3D;
        using Scene3D = Magnum::SceneGraph::Scene<MatTransform3D>;
        using Object3D = Magnum::SceneGraph::Object<MatTransform3D>;

        class ShaderPack2 {
            std::optional<Shaders::Flat2D> flat;
            std::optional<Shaders::VertexColor2D> vertex_color;
        };
        class ShaderPack3 {
            std::optional<Shaders::Flat3D> flat;
            std::optional<Shaders::VertexColor3D> vertex_color;
            std::optional<Shaders::Phong> phong;
            std::optional<Shaders::MeshVisualizer> mesh_visualizer;
        };
}
