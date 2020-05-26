
#pragma once

#include "mtao/opengl/objects/types.h"

namespace mtao::opengl {

class Scene2D : public Magnum::SceneGraph::Scene<MatTransform2D> {
    Magnum::Vector2 cameraPosition(
        const Magnum::Vector2i& position) const;  //[0,1]^2 position of the camera
    // position in 2D space with respect to the root node
    Magnum::Vector2 localPosition(const Magnum::Vector2i& position) const;
    Object2D& root() { return _root; }
    Magnum::SceneGraph::Camera2D& camera() { return _camera; }
    const Object2D& root() const { return _root; }
    const Magnum::SceneGraph::Camera2D& camera() const { return _camera; }
    const Magnum::SceneGraph::DrawableGroup2D& drawables() const {
        return _drawables;
    }
    void updateTransformation();

   private:
    Object2D _root, _cameraObject;
    Camera2D _camera;
    Magnum::Vector2 translation;
    float scale = 1.0;
};

class Scene3D : public Magnum::SceneGraph::Scene<MatTransform3D> {
    Magnum::Vector3 positionOnSphere(const Magnum::Vector2i& position) const;

    Object3D& root() { return _root; }
    Object3D& scene() { return _scene; }
    Magnum::SceneGraph::Camera3D& camera() { return _camera; }
    Magnum::SceneGraph::DrawableGroup3D& drawables() { return _drawables; }

   private:
    Scene3D _scene;
    Object3D _root;
    Camera3D _camera;
}

}  // namespace mtao::opengl
