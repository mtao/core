
#pragma once

#include "mtao/opengl/objects/types.h"

namespace mtao::opengl {

class Scene2D : public Magnum::SceneGraph::Scene<MatTransform2D> {
    Magnum::Vector2 cameraPosition(const Magnum::Vector2i& position)
        const;  //[0,1]^2 position of the camera
    // position in 2D space with respect to the root node
    Magnum::Vector2 localPosition(const Magnum::Vector2i& position) const;
    Object2D& root() { return _root; }
    Magnum::SceneGraph::Camera2D& camera() { return _camera; }
    const Object2D& root() const { return _root; }
    const Magnum::SceneGraph::Camera2D& camera() const { return *_camera; }
    void updateTransformation();

   private:
    Object2D _root;
    std::shared_ptr<Camera2D> _camera;
};

class Scene3D : public Magnum::SceneGraph::Scene<MatTransform3D> {
    Object3D& root() { return _root; }
    Magnum::SceneGraph::Camera3D& camera() { return *_camera; }

   private:
    Object3D _root;
    std::shared_ptr<Camera3D> _camera;
};

}  // namespace mtao::opengl
