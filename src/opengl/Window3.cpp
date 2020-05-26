#include <imgui.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "mtao/opengl/Window.h"
#ifdef MTAO_HAS_LIBPNGPP
#include <png++/png.hpp>
#endif
#include <Magnum/GL/DebugOutput.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>

#include <iomanip>
#include <iostream>
#include <mtao/logging/logger.hpp>

using namespace Magnum;
using namespace Math::Literals;

namespace mtao::opengl {
Window3::Window3(const Arguments& args, GL::Version version)
    : WindowBase(args, version) {}
void Window3::draw() { scene().camera().draw(_drawables); }
void Window3::viewportEvent(ViewportEvent& event) {
    WindowBase::viewportEvent(event);
    scene().setViewport(event.windowSize());
}

void Window3::mousePressEvent(MouseEvent& event) {
    WindowBase::mousePressEvent(event);
    if (!ImGui::GetIO().WantCaptureMouse) {
        if (event.button() == MouseEvent::Button::Right)
            _previousPosition = scene.positionOnSphere(event.position());
    }
}

void Window3::mouseReleaseEvent(MouseEvent& event) {
    WindowBase::mouseReleaseEvent(event);
    if (event.button() == MouseEvent::Button::Right)
        _previousPosition = Vector3();
}

void Window3::mouseScrollEvent(MouseScrollEvent& event) {
    WindowBase::mouseScrollEvent(event);
    if (!ImGui::GetIO().WantCaptureMouse) {
        if (!event.offset().y()) return;

        /* Distance to origin */
        const Float distance = _cameraObject.transformation().translation().z();

        /* Move 15% of the distance back or forward */
        _cameraObject.translate(Vector3::zAxis(
            distance * (1.0f - (event.offset().y() > 0 ? 1 / 0.85f : 0.85f))));

        redraw();
    }
}

void Window3::mouseMoveEvent(MouseMoveEvent& event) {
    WindowBase::mouseMoveEvent(event);
    if (!(event.buttons() & MouseMoveEvent::Button::Right)) return;

    const Vector3 currentPosition = positionOnSphere(event.position());
    const Vector3 axis = Math::cross(_previousPosition, currentPosition);

    if (_previousPosition.length() < 0.001f || axis.length() < 0.001f) return;

    _root.rotate(Math::angle(_previousPosition, currentPosition),
                 axis.normalized());
    _previousPosition = currentPosition;

    redraw();
}
}  // namespace mtao::opengl
