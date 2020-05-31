#include "mtao/opengl/scene.h"

namespace mtao::opengl {
Scene3D::Scene3D() {
    _camera->setParent(&_scene).translate(Vector3::zAxis(5.0f));
    _camera.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(
            Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    _root.setParent(&_scene);
}
void Scene3D::setViewport(const Magnum::Vector2i& windowSize) {
    _camera.setViewport(windowSize);
}
}  // namespace mtao::opengl
