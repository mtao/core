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
    Window2::Window2(const Arguments& args): WindowBase(args), _cameraObject(&_scene), _camera(_cameraObject) {
        _cameraObject.setParent(&_scene);
        _camera.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix3::projection({1.0f, 1.0f}))
            .setViewport(GL::defaultFramebuffer.viewport().size());


        _root.setParent(&_scene);
    }
    void Window2::draw() {
        _camera.draw(_drawables);
    }
    void Window2::viewportEvent(ViewportEvent& event) {
        WindowBase::viewportEvent(event);
        _camera.setViewport(event.windowSize());
    }

    void Window2::mousePressEvent(MouseEvent& event) {
        WindowBase::mousePressEvent(event);
        if(!ImGui::GetIO().WantCaptureMouse) {
            if(event.button() == MouseEvent::Button::Right)
                _previousPosition = cameraPosition(event.position());
        }
    }

    void Window2::mouseReleaseEvent(MouseEvent& event) {
        WindowBase::mouseReleaseEvent(event);
        if(event.button() == MouseEvent::Button::Right)
            _previousPosition = Vector2();
    }
    auto Window2::cameraPosition(const Vector2i& position) const -> Vector2 {
        const Vector2 positionNormalized = Vector2{position}/Vector2{_camera.viewport()};
        return positionNormalized;
    }
    auto Window2::localPosition(const Vector2i& position) const  -> Vector2 {
                auto S = windowSize();
                Magnum::Vector3 V(cameraPosition(position),1);
                V.x() -= .5f;
                V.y() -= .5f;

                V.x() *= float(S.x()) / float(S.y());
                V.y() *= -1;
                V = cameraObject().transformation()* V;
                return {V.x(),V.y()};
    }

    void Window2::mouseScrollEvent(MouseScrollEvent& event) {
        WindowBase::mouseScrollEvent(event);
        if(!ImGui::GetIO().WantCaptureMouse) {
            //_camera.setProjectionMatrix(Matrix3::projection({20.0f/scale, 20.0f/scale}));
            float my_scale= (event.offset().y() > 0 ? 1/0.85f : 0.85f);
            scale *= my_scale;
            updateTransformation();
            redraw();
        }
    }


    void Window2::mouseMoveEvent(MouseMoveEvent& event) {
        WindowBase::mouseMoveEvent(event);
        if(!(event.buttons() & MouseMoveEvent::Button::Right)) return;

        const Vector2 currentPosition = cameraPosition(event.position());

        if(_previousPosition.length() < 0.001f) return;

        Vector2 diff = currentPosition - _previousPosition;
        auto S = windowSize();
        diff.y() *= -1;
        diff.x() /= float(S.y()) / float(S.x());
        translation -= diff;

        _previousPosition = currentPosition;
        updateTransformation();

        redraw();
    }

    void Window2::updateTransformation() {

            _cameraObject.resetTransformation().translate(translation).scale({scale,scale});
    }
}
