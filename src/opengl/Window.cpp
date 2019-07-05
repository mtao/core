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
    WindowBase::WindowBase(const Arguments& arguments): 
        GlfwApplication{arguments, Configuration{}.setTitle("WindowBase").setWindowFlags(Configuration::WindowFlag::Resizable)},
        _imgui{Vector2(windowSize())/dpiScaling(), windowSize(), framebufferSize()}
    {
        //TODO: This is certainly not what I want, but isn't the imgui constructor supposed to do things for me?
        ImGui::GetIO().FontGlobalScale =  1.0 / dpiScaling()[0] / dpiScaling()[1];


        GL::Renderer::enable(GL::Renderer::Feature::DebugOutput);
        GL::Renderer::enable(GL::Renderer::Feature::DebugOutputSynchronous);
        GL::DebugOutput::setDefaultCallback();
        GL::DebugOutput::setEnabled(
                GL::DebugOutput::Source::Api, GL::DebugOutput::Type::Other, {131185}, false);
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        setSwapInterval(1);
        //setMinimalLoopPeriod(16);
    }
    void WindowBase::drawEvent() {


        {//ImGui
            _imgui.newFrame();
            if(ImGui::GetIO().WantTextInput && !isTextInputActive())
                startTextInput();
            else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
                stopTextInput();

            gui();

        }
        {
            GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);
            draw();

            //setup
            GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
                    GL::Renderer::BlendEquation::Add);
            GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                    GL::Renderer::BlendFunction::OneMinusSourceAlpha);
            GL::Renderer::enable(GL::Renderer::Feature::Blending);
            GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
            GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
            GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);


            _imgui.drawFrame();

            //Cleanup
            GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
            GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
            GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
            GL::Renderer::disable(GL::Renderer::Feature::Blending);
        }

        swapBuffers();
        redraw();
    }







    void WindowBase::viewportEvent(ViewportEvent& event) {
        GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

        _imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
                event.windowSize(), event.framebufferSize());
    }

    void WindowBase::keyPressEvent(KeyEvent& event) {
        if(_imgui.handleKeyPressEvent(event)) {
            redraw();
            return;
        }
    }

    void WindowBase::keyReleaseEvent(KeyEvent& event) {
        if(_imgui.handleKeyReleaseEvent(event)) {
            redraw();
            return;
        }
    }

    void WindowBase::mousePressEvent(MouseEvent& event) {
        if(_imgui.handleMousePressEvent(event)) {
            redraw();
            return;
        }
    }

    void WindowBase::mouseReleaseEvent(MouseEvent& event) {
        if(_imgui.handleMouseReleaseEvent(event)) {
            redraw();
            return;
        }
    }

    void WindowBase::mouseMoveEvent(MouseMoveEvent& event) {
        if(_imgui.handleMouseMoveEvent(event)) {
            redraw();
            return;
        }
    }

    void WindowBase::mouseScrollEvent(MouseScrollEvent& event) {
        if(_imgui.handleMouseScrollEvent(event)) {
            redraw();
            return;
        }
    }

    void WindowBase::textInputEvent(TextInputEvent& event) {
        if(_imgui.handleTextInputEvent(event)) {
            redraw();
            return;
        }
    }






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
                _previousPosition = localPosition(event.position());
        }
    }

    void Window2::mouseReleaseEvent(MouseEvent& event) {
        WindowBase::mouseReleaseEvent(event);
        if(event.button() == MouseEvent::Button::Right)
            _previousPosition = Vector2();
    }
    Vector2 Window2::localPosition(const Vector2i& position) const {
        const Vector2 positionNormalized = Vector2{position}/Vector2{_camera.viewport()};
        return positionNormalized;
    }

    void Window2::mouseScrollEvent(MouseScrollEvent& event) {
        WindowBase::mouseScrollEvent(event);
        if(!ImGui::GetIO().WantCaptureMouse) {
            _camera.setProjectionMatrix(Matrix3::projection({20.0f/scale, 20.0f/scale}));
            float my_scale= (event.offset().y() > 0 ? 1/0.85f : 0.85f);
            scale *= my_scale;
            _root.scaleLocal({my_scale,my_scale});
            redraw();
        }
    }


    void Window2::mouseMoveEvent(MouseMoveEvent& event) {
        WindowBase::mouseMoveEvent(event);
        if(!(event.buttons() & MouseMoveEvent::Button::Right)) return;

        const Vector2 currentPosition = localPosition(event.position());

        if(_previousPosition.length() < 0.001f) return;

        Vector2 diff = currentPosition - _previousPosition;
        auto S = windowSize();
        diff.y() *= -1;
        diff.x() /= float(S.y()) / float(S.x());
        const auto& T = _root.transformation();
        diff.x() *=  T[0][0];
        diff.y() *=  T[1][1];
        _root.translate(diff);
        _previousPosition = currentPosition;

        redraw();
    }



}




