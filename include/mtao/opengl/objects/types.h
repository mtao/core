#pragma once
#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>

namespace mtao::opengl {
using MatTransform2D = Magnum::SceneGraph::MatrixTransformation2D;
// using Scene2D = Magnum::SceneGraph::Scene<MatTransform2D>;
using Object2D = Magnum::SceneGraph::Object<MatTransform2D>;

using MatTransform3D = Magnum::SceneGraph::MatrixTransformation3D;
// using Scene3D = Magnum::SceneGraph::Scene<MatTransform3D>;
using Object3D = Magnum::SceneGraph::Object<MatTransform3D>;

class Camera2D : public Object2D, public Magnum::SceneGraph::Camera2D {
    inline Camera2D()
        : Magnum::SceneGraph::Camera2D(*static_cast<Object2D*>(this)) {}
};

class Camera3D : public Object3D, public Magnum::SceneGraph::Camera3D {
    inline Camera3D()
        : Magnum::SceneGraph::Camera3D(*static_cast<Object3D*>(this)) {}
};

}  // namespace mtao::opengl
