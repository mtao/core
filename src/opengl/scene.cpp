#include "mtao/opengl/scene.h"

namespace mtao::opengl {
Scene3D::Scene3D() : _cameraObject(this), _camera(_cameraObject) {
    _cameraObject.setParent(&_scene).translate(Vector3::zAxis(5.0f));
    _camera.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(
            Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    _root.setParent(&_scene);
}
void Scene3D::setViewport(const Magnum::Vector2i& windowSize) {
    _camera.setViewport(windowSize);
}
Magnum::Vector3 Scene3D::positionOnSphere(const Vector2i& position) const {
    const Vector2 positionNormalized =
        Vector2{position} / Vector2{_camera.viewport()} - Vector2{0.5f};
    const Float length = positionNormalized.length();
    const Vector3 result(length > 1.0f
                             ? Vector3(positionNormalized, 0.0f)
                             : Vector3(positionNormalized, 1.0f - length));
    return (result * Vector3::yScale(-1.0f)).normalized();
}
}  // namespace mtao::opengl
