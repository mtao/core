
#pragma once
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Object.h>

#include "mtao/opengl/objects/types.h"

namespace mtao::opengl {

class Camera2D : public Object2D, public Magnum::SceneGraph::Camera2D {
    inline Camera2D()
        : Magnum::SceneGraph::Camera2D(*static_cast<Object2D*>(this)) {}

    void updateTransformation();
    void setFromOther(const Camera3D& other);

    // parameterization for the camera
    Magnum::Vector2 translation;
    float scale = 1.0;
};

class Camera3D : public Object3D, public Magnum::SceneGraph::Camera3D {
    inline Camera3D()
        : Magnum::SceneGraph::Camera3D(*static_cast<Object3D*>(this)) {}
    void setFromOther(const Camera3D& other);

    virtual void updatePositionFromScreen(const Magnum::Vector2i& position);

    // onM
    virtual void onMousePress(const Magnum::Vector2i& position);
};

class CenterPointingCamera3D : public Camera3D {
    Magnum::Vector3 positionOnSphere(const Magnum::Vector2i& position) const;

    void updateSpherePosition(const Magnum::Vector3& position);
    void setSpherePosition(const Magnum::Vector3& position);

    // pass in a mouse position
    void updateSpherePosition(const Magnum::Vector2i& position);
    void setSpherePosition(const Magnum::Vector2i& position);

    // the last place on
    Magnum::Vector3 _spherePosition;
};

}  // namespace mtao::opengl
