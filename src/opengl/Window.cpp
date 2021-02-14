#include "mtao/opengl/Window.h"
#include <Magnum/GL/BufferImage.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <Corrade/Containers/StridedArrayView.h>
#include <Magnum/PixelFormat.h>
#include <iostream>
#include <misc/cpp/imgui_stdlib.h>
#include <sstream>
#include <stdexcept>
#include <imgui.h>
#include <vector>
#ifdef MTAO_HAS_LIBPNGPP
        #include <png++/image.hpp>
#include <png++/rgb_pixel.hpp>
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
    spdlog::debug("Searching for a png support in Magnum plugin dirs:: {}", fmt::join(Corrade::PluginManager::AbstractPlugin::pluginSearchPaths(), ","));
#if defined(MTAO_HAS_MAGNUM_PNGIMAGECONVERTER)
    CORRADE_RESOURCE_INITIALIZE(PngImageConverter);
    CORRADE_PLUGIN_IMPORT(PngImageConverter);
    if (!(manager->load("PngImageConverter") & PluginManager::LoadState::Loaded)) {
        Utility::Error{} << "The requested plugin"
                         << "PngImageConverter"
                         << "cannot be loaded.";
    }
#endif
    spdlog::info("available plugin aliases:: {}", fmt::join(manager->aliasList(), ","));
    return manager;
}
namespace {

Corrade::Containers::Pointer<PluginManager::Manager<Magnum::Trade::AbstractImageConverter>> manager = get_manager();
auto get_pngimporter() {
    auto pluginlist = manager->aliasList();
    Containers::Pointer<Magnum::Trade::AbstractImageConverter> ret;
    bool found = false;
    for (auto &&p : pluginlist) {
        if (p == "PngImageConverter") {
            found = true;
            break;
        }
    }
    if (found) {
        ret = manager->instantiate("PngImageConverter");
    }
    return ret;
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

        if (_recording_dirty && !_recording_includes_gui) {
            record_frame_to_file();
        }

        _imgui.drawFrame();

        if (_recording_dirty && _recording_includes_gui) {
            record_frame_to_file();
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

void WindowBase::set_recording_path(const std::string &path) {
    _recording_path = path;
}

void WindowBase::recording_gui() {
    ImGui::Begin("Recording");
    /*
    if (!_image_saver) {
        ImGui::Text("Was unable to load image loader plugin so no recording :(. Buttons are essentialy no-ops.");
    }
    */
    if (ImGui::Button("Capture Single Frame")) {
        _keep_recording = false;
        _recording_dirty = true;
    }
    if (ImGui::Checkbox("Capture", &_keep_recording)) {
        _recording_dirty = _keep_recording;
    }
    //if (_recording_active) {
    //    setMinWindowSize(framebufferSize());
    //    setMaxWindowSize(framebufferSize());
    //}
    ImGui::InputText("Output format", &_recording_filename_format);
    ImGui::InputInt("Frame index", &_recording_index);
    ImGui::Checkbox("Include GUI", &_recording_includes_gui);
    ImGui::Checkbox("Automatically increment frame indices", &_auto_increment);


    ImGui::End();
}
void WindowBase::automatically_increment_recording_frames(bool do_it) {
    _auto_increment = do_it;
}
Magnum::Image2D WindowBase::current_frame() {
    auto &fb = Magnum::GL::defaultFramebuffer;
    return fb.read(fb.viewport(), Magnum::Image2D{ Magnum::PixelFormat::RGBA8Unorm });
}
void WindowBase::record_frame_to_file() {
        std::string filename = std::filesystem::path(_recording_path) / fmt::format(_recording_filename_format.c_str(), _recording_index);
        auto img = current_frame();

        if (_image_saver && _image_saver->exportToFile(img, filename)) {
            spdlog::debug("Successfully wrote frame to file {}", filename);
        } else {
#if defined(MTAO_HAS_LIBPNGPP)
            png::image< png::rgb_pixel > image(img.size()[0],img.size()[1]);
            auto pixels = img.pixels();
            for (png::uint_32 y = 0; y < image.get_height(); ++y)
            {
                    auto s = pixels[image.get_height() - y - 1];
                for (png::uint_32 x = 0; x < image.get_width(); ++x)
                {
                    const auto& p = s[x];
                    image[y][x] = png::rgb_pixel(p[0],p[1],p[2]);
                    // non-checking equivalent of image.set_pixel(x, y, ...);
                }
            }
            image.write(std::string(filename));

            spdlog::warn("Wrote a frame with png++ to file {}!", filename);
#else
            spdlog::error("FAiled to write an image because no output engine found (neither libpngpp or MagnumPlugins");
#endif
        }
    if (_auto_increment) {
        increment_recording_frame_index();
    }
    if (!_keep_recording) {
        _recording_dirty = false;
    }
}

void WindowBase::set_recording_frame_callback(std::optional<std::function<bool(int)>> f) {
    _recording_set_frame_callback = std::move(f);
}
void WindowBase::increment_recording_frame_index() {
    set_recording_frame_index(_recording_index + 1);
}
void WindowBase::reset_recording_frame_index() {
    set_recording_frame_index(0);
}
void WindowBase::set_recording_frame_index(int index) {
    int old_index = _recording_index;
    _recording_index = index;
    if (_keep_recording) {
        _recording_dirty = true;
    }
    if (_recording_set_frame_callback) {
        if (!(*_recording_set_frame_callback)(_recording_index)) {
            _recording_index = old_index;
            _keep_recording = false;
        }
    }
}

}// namespace mtao::opengl
