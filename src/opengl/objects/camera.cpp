#include "mtao/opengl/objects/camera.h"

namespace mtao::opengl {
void Camera2D::setFromOther(const Camera2D& other) {
    Magnum::SceneGraph::Camera2D::operator=(other);
}
void Camera3D::setFromOther(const Camera3D& other) {
    Magnum::SceneGraph::Camera3D::operator=(other);
}

void CenterPointingCamera3D::updatePosition(const Magnum::Vector3& position) {
    setPosition(position);
}
void CenterPointingCamera3D::setPosition(const Magnum::Vector3& position) {
    _position = position;
}
void CenterPointingCamera3D::updatePosition(const Magnum::Vector2i& position) {
    updatePosition(pointOnSphere(position));
}
void CenterPointingCamera3D::setPosition(const Magnum::Vector2i& position) {
    setPosition(pointOnSphere(position));
}
Magnum::Vector3 CenterPointingCamera3D::positionOnSphere(
    const Vector2i& position) const {
    const Vector2 positionNormalized =
        Vector2{position} / Vector2{viewport()} - Vector2{0.5f};
    const Float length = positionNormalized.length();
    const Vector3 result(length > 1.0f
                             ? Vector3(positionNormalized, 0.0f)
                             : Vector3(positionNormalized, 1.0f - length));
    return (result * Vector3::yScale(-1.0f)).normalized();
}
}  // namespace mtao::opengl
