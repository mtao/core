#include "mtao/opengl/Window.h"
#include <Magnum/GL/BufferImage.h>
#include <spdlog/spdlog.h>
#include <Magnum/PixelFormat.h>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>
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
#include <Magnum/ImageView.h>
#include <Magnum/GL/DebugOutput.h>
#include <Magnum/Trade/AbstractImageConverter.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Magnum/Math/Color.h>
#include <iostream>
#include <Magnum/GL/Extensions.h>
#include <fmt/format.h>


using namespace Magnum;
using namespace Math::Literals;

auto get_manager() {
    Corrade::Containers::Pointer<PluginManager::Manager<Magnum::Trade::AbstractImageConverter>> manager;
    manager.emplace();
    CORRADE_RESOURCE_INITIALIZE(PngImageConverter);
    spdlog::info("Searching for a png support in Magnum plugin dirs:: {}", fmt::join(Corrade::PluginManager::AbstractPlugin::pluginSearchPaths(), ","));
    CORRADE_PLUGIN_IMPORT(PngImageConverter);
    if (!(manager->load("PngImageConverter") & PluginManager::LoadState::Loaded)) {
        Utility::Error{} << "The requested plugin"
                         << "PngImageConverter"
                         << "cannot be loaded.";
    }
    spdlog::info("available plugin aliases:: {}", fmt::join(manager->aliasList(), ","));
    return manager;
}
namespace {

Corrade::Containers::Pointer<PluginManager::Manager<Magnum::Trade::AbstractImageConverter>> manager = get_manager();
auto get_pngimporter() {
    Containers::Pointer<Magnum::Trade::AbstractImageConverter> image = manager->instantiate("PngImageConverter");
    return image;
}
}// namespace

namespace mtao::opengl {
WindowBase::WindowBase(const Arguments &arguments, GL::Version version) : GlfwApplication{ arguments, Configuration{}.setTitle("WindowBase").setWindowFlags(Configuration::WindowFlag::Resizable), GLConfiguration{}.setVersion(version) },
                                                                          _imgui{ Vector2(windowSize()) / dpiScaling(), windowSize(), framebufferSize() } {
    //TODO: This is certainly not what I want, but isn't the imgui constructor supposed to do things for me?
    ImGui::GetIO().FontGlobalScale = 1.0 / dpiScaling()[0] / dpiScaling()[1];


    GL::Renderer::enable(GL::Renderer::Feature::DebugOutput);
    GL::Renderer::enable(GL::Renderer::Feature::DebugOutputSynchronous);
    GL::DebugOutput::setDefaultCallback();
    GL::DebugOutput::setEnabled(
      GL::DebugOutput::Source::Api, GL::DebugOutput::Type::Other, { 131185 }, false);
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    setSwapInterval(1);


    _image_saver = ::get_pngimporter();
    //setMinimalLoopPeriod(16);
}
void WindowBase::draw() {}
void WindowBase::drawEvent() {


    {//ImGui
        _imgui.newFrame();
        if (ImGui::GetIO().WantTextInput && !isTextInputActive()) {
            startTextInput();
        } else if (!ImGui::GetIO().WantTextInput && isTextInputActive()) {
            stopTextInput();
        }

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

        if (_recording_active && !_recording_includes_gui) {
            record_frame();
        }

        _imgui.drawFrame();

        if (_recording_active && _recording_includes_gui) {
            record_frame();
        }

        //Cleanup
        GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        GL::Renderer::disable(GL::Renderer::Feature::Blending);
    }

    swapBuffers();
    redraw();
}


void WindowBase::viewportEvent(ViewportEvent &event) {
    GL::defaultFramebuffer.setViewport({ {}, event.framebufferSize() });

    _imgui.relayout(Vector2{ event.windowSize() } / event.dpiScaling(),
                    event.windowSize(),
                    event.framebufferSize());
}

void WindowBase::keyPressEvent(KeyEvent &event) {
    if (_imgui.handleKeyPressEvent(event)) {
        event.setAccepted();
        redraw();
        return;
    }
}

void WindowBase::keyReleaseEvent(KeyEvent &event) {
    if (_imgui.handleKeyReleaseEvent(event)) {
        event.setAccepted();
        redraw();
        return;
    }
}

void WindowBase::mousePressEvent(MouseEvent &event) {
    if (_imgui.handleMousePressEvent(event)) {
        event.setAccepted();
        redraw();
        return;
    }
}

void WindowBase::mouseReleaseEvent(MouseEvent &event) {
    if (_imgui.handleMouseReleaseEvent(event)) {
        event.setAccepted();
        redraw();
        return;
    }
}

void WindowBase::mouseMoveEvent(MouseMoveEvent &event) {
    if (_imgui.handleMouseMoveEvent(event)) {
        event.setAccepted();
        redraw();
        return;
    }
}

void WindowBase::mouseScrollEvent(MouseScrollEvent &event) {
    if (_imgui.handleMouseScrollEvent(event)) {
        event.setAccepted();
        redraw();
        return;
    }
}

void WindowBase::textInputEvent(TextInputEvent &event) {
    if (_imgui.handleTextInputEvent(event)) {
        event.setAccepted();
        redraw();
        return;
    }
}


bool WindowBase::supportsGeometryShader() const {
    using namespace Magnum;
#ifndef MAGNUM_TARGET_GLES
    using ShaderType = GL::Extensions::ARB::geometry_shader4;
#elif !defined(MAGNUM_TARGET_WEBGL)
    using ShaderType = GL::Extensions::EXT::geometry_shader;
#endif

    return isExtensionSupported<ShaderType>();
}


void WindowBase::recording_gui() {
    ImGui::Begin("Recording");
    if (ImGui::Button("Capture Single Frame")) {
        _keep_recording = false;
        _recording_active = true;
    }
    if (ImGui::Checkbox("Capture", &_recording_active)) {
        _keep_recording = true;
    }
    ImGui::InputText("Output format", &_recording_filename_format);
    ImGui::Checkbox("Include GUI", &_recording_includes_gui);


    ImGui::End();
}
Magnum::Image2D WindowBase::current_frame() {
    auto &fb = Magnum::GL::defaultFramebuffer;
    return fb.read(fb.viewport(), Magnum::Image2D{ Magnum::PixelFormat::RGBA8Unorm });
}
void WindowBase::record_frame() {
    std::string filename = fmt::format(_recording_filename_format.c_str(), _recording_index);
    auto img = current_frame();
    if (_image_saver->exportToFile(img, filename)) {
        spdlog::debug("Successfully wrote frame to file {}", filename);
    _recording_index++;
    } else {
        spdlog::warn("Failed to write frame to file {}!", filename);
    }
    if (!_keep_recording) {
        _recording_active = false;
    }
}

}// namespace mtao::opengl
