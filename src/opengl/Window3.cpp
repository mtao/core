#include "mtao/opengl/Window.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <imgui.h>
#include <vector>
#ifdef MTAO_HAS_LIBPNGPP
#include <png++/png.hpp>
#endif
#include <iomanip>
#include <mtao/logging/logger.hpp>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/DebugOutput.h>
#include <Magnum/Math/Color.h>
#include <iostream>


using namespace Magnum;
using namespace Math::Literals;

namespace mtao::opengl {
    Window3::Window3(const Arguments& args): WindowBase(args), _cameraObject(&_scene), _camera(_cameraObject) {
        _cameraObject.setParent(&_scene)
            .translate(Vector3::zAxis(5.0f));
        _camera.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
            .setViewport(GL::defaultFramebuffer.viewport().size());

        _root.setParent(&_scene);
    }
    void Window3::draw() {
        _camera.draw(_drawables);
    }
    void Window3::viewportEvent(ViewportEvent& event) {
        WindowBase::viewportEvent(event);
        _camera.setViewport(event.windowSize());
    }
    Vector3 Window3::positionOnSphere(const Vector2i& position) const {
        const Vector2 positionNormalized = Vector2{position}/Vector2{_camera.viewport()} - Vector2{0.5f};
        const Float length = positionNormalized.length();
        const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
        return (result*Vector3::yScale(-1.0f)).normalized();
    }


    void Window3::mousePressEvent(MouseEvent& event) {
        WindowBase::mousePressEvent(event);
        if(!ImGui::GetIO().WantCaptureMouse) {
            if(event.button() == MouseEvent::Button::Right)
                _previousPosition = positionOnSphere(event.position());
        }
    }

    void Window3::mouseReleaseEvent(MouseEvent& event) {
        WindowBase::mouseReleaseEvent(event);
        if(event.button() == MouseEvent::Button::Right)
            _previousPosition = Vector3();
    }

    void Window3::mouseScrollEvent(MouseScrollEvent& event) {
        WindowBase::mouseScrollEvent(event);
        if(!ImGui::GetIO().WantCaptureMouse) {
            if(!event.offset().y()) return;

            /* Distance to origin */
            const Float distance = _cameraObject.transformation().translation().z();

            /* Move 15% of the distance back or forward */
            _cameraObject.translate(Vector3::zAxis(
                        distance*(1.0f - (event.offset().y() > 0 ? 1/0.85f : 0.85f))));

            redraw();
        }
    }


    void Window3::mouseMoveEvent(MouseMoveEvent& event) {
        WindowBase::mouseMoveEvent(event);
        if(!(event.buttons() & MouseMoveEvent::Button::Right)) return;

        const Vector3 currentPosition = positionOnSphere(event.position());
        const Vector3 axis = Math::cross(_previousPosition, currentPosition);

        if(_previousPosition.length() < 0.001f || axis.length() < 0.001f) return;

        _root.rotate(Math::angle(_previousPosition, currentPosition), axis.normalized());
        _previousPosition = currentPosition;

        redraw();
    }
}
