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
#include <Magnum/Math/Color.h>


using namespace Magnum;
using namespace Math::Literals;

namespace mtao::opengl {
    Window::Window(const Arguments& arguments): 
        GlfwApplication{arguments, Configuration{}.setTitle("Window").setWindowFlags(Configuration::WindowFlag::Resizable)},
        _imgui{Vector2(windowSize())/dpiScaling(), windowSize(), framebufferSize()}
    {
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        setSwapInterval(1);
        //setMinimalLoopPeriod(16);
    }
    Window3::Window3(const Arguments& args): Window(args), _cameraObject(&_scene), _camera(_cameraObject) {
        _cameraObject.setParent(&_scene)
            .translate(Vector3::zAxis(5.0f));
        _camera.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
            .setViewport(GL::defaultFramebuffer.viewport().size());

        _root.setParent(&_scene);
    }
    void Window::drawEvent() {


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

        void Window3::draw() {
            _camera.draw(_drawables);
        }






    void Window::viewportEvent(ViewportEvent& event) {
        GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

        _imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
                event.windowSize(), event.framebufferSize());
    }
    void Window3::viewportEvent(ViewportEvent& event) {
        Window::viewportEvent(event);
        _camera.setViewport(event.windowSize());
    }

    void Window::keyPressEvent(KeyEvent& event) {
        if(_imgui.handleKeyPressEvent(event)) {
            redraw();
            return;
        }
    }

    void Window::keyReleaseEvent(KeyEvent& event) {
        if(_imgui.handleKeyReleaseEvent(event)) {
            redraw();
            return;
        }
    }

    void Window::mousePressEvent(MouseEvent& event) {
        if(_imgui.handleMousePressEvent(event)) {
            redraw();
            return;
        }
    }

    void Window::mouseReleaseEvent(MouseEvent& event) {
        if(_imgui.handleMouseReleaseEvent(event)) {
            redraw();
            return;
        }
    }

    void Window::mouseMoveEvent(MouseMoveEvent& event) {
        if(_imgui.handleMouseMoveEvent(event)) {
            redraw();
            return;
        }
    }

    void Window::mouseScrollEvent(MouseScrollEvent& event) {
        if(_imgui.handleMouseScrollEvent(event)) {
            redraw();
            return;
        }
    }

    void Window::textInputEvent(TextInputEvent& event) {
        if(_imgui.handleTextInputEvent(event)) {
            redraw();
            return;
        }
    }
    Vector3 Window3::positionOnSphere(const Vector2i& position) const {
        const Vector2 positionNormalized = Vector2{position}/Vector2{_camera.viewport()} - Vector2{0.5f};
        const Float length = positionNormalized.length();
        const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
        return (result*Vector3::yScale(-1.0f)).normalized();
    }


    void Window3::mousePressEvent(MouseEvent& event) {
        Window::mousePressEvent(event);
        if(!ImGui::GetIO().WantCaptureMouse) {
            if(event.button() == MouseEvent::Button::Left)
                _previousPosition = positionOnSphere(event.position());
        }
    }

    void Window3::mouseReleaseEvent(MouseEvent& event) {
        Window::mouseReleaseEvent(event);
        if(event.button() == MouseEvent::Button::Left)
            _previousPosition = Vector3();
    }

    void Window3::mouseScrollEvent(MouseScrollEvent& event) {
        Window::mouseScrollEvent(event);
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
        Window::mouseMoveEvent(event);
        if(!(event.buttons() & MouseMoveEvent::Button::Left)) return;

        const Vector3 currentPosition = positionOnSphere(event.position());
        const Vector3 axis = Math::cross(_previousPosition, currentPosition);

        if(_previousPosition.length() < 0.001f || axis.length() < 0.001f) return;

        _root.rotate(Math::angle(_previousPosition, currentPosition), axis.normalized());
        _previousPosition = currentPosition;

        redraw();
    }





















    /*
       size_t Window::s_window_count = 0;
       std::map<GLFWwindow*,HotkeyManager> Window::s_hotkeys;

       static void error_callback(int error, const char* description)
       {
       std::stringstream ss;
       ss << "Error: " << description << std::endl;
       throw std::runtime_error(ss.str());
       }


       void printGLInfo() {
       logging::info() << "OpenGL Version: " << glGetString(GL_VERSION);

       logging::info() << "OpenGL Vendor: " << glGetString(GL_VENDOR);

       logging::info() << "OpenGL Renderer: " <<  glGetString(GL_RENDERER);
       }

       Window::Window( const std::string& name, int width, int height) {

       if (s_window_count++ == 0 && !glfwInit()) {
       std::cerr <<" GLFWInit faillure!" << std::endl;
       exit(EXIT_FAILURE);
       }


       glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
       glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
setErrorCallback(error_callback);
if (!window) {
glfwTerminate();
throw std::runtime_error("GLFW window creation failed!");
}
s_hotkeys[window];
makeCurrent();


// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
bool err = gladLoadGL() == 0;
#else
bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
if(err) {
logging::fatal() << "OpenGL loader/wrangler failed to load!";
}
printGLInfo();
m_gui.setWindow(window);


glfwSetMouseButtonCallback(window, ImGuiImpl::mouseButtonCallback);
glfwSetScrollCallback(window, ImGuiImpl::scrollCallback);
glfwSetKeyCallback(window,    Window::keyCallback);
glfwSetCharCallback(window,   ImGuiImpl::charCallback);


glfwSwapInterval(1);
}

Window::~Window() {
    s_window_count--;
    if(window) {
        glfwDestroyWindow(window);
    }
#if !defined(USE_IMGUI_IMPL)
    if(s_window_count == 0) {
        glfwTerminate();
    }
#endif
}

#if !defined(USE_IMGUI_IMPL)
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
#endif

void Window::run() {
    while (!glfwWindowShouldClose(window)) {
        draw();
    }
}

void Window::draw(bool show_gui) {
    //ImGuiIO& io = ImGui::GetIO();
    makeCurrent();
    glfwPollEvents();

    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"

    int display_w, display_h;
    glfwGetFramebufferSize(window,&display_w, &display_h);
    glEnable(GL_MULTISAMPLE);
    if(m_gui_func) {
        m_gui.newFrame();
        m_gui_func();
    }
    if(m_render_func) {
        m_render_func(display_w,display_h);
    }
    if(m_gui_func && show_gui) {
        m_gui.render();
    }
    glfwSwapBuffers(window);
    if(m_is_recording) {
        save_frame();
    }

}

void Window::setScrollCallback(GLFWscrollfun f) {
    makeCurrent();
    glfwSetScrollCallback(window, f);
}

void Window::setKeyCallback(GLFWkeyfun f) {
    makeCurrent();
    glfwSetKeyCallback(window, f);
}
void Window::setErrorCallback(GLFWerrorfun  f) {
    makeCurrent();
    glfwSetErrorCallback(f);
}
void Window::makeCurrent() {
    glfwMakeContextCurrent(window);
}
HotkeyManager& Window::hotkeys() {
    return s_hotkeys.at(window);
}
const HotkeyManager& Window::hotkeys() const {
    return s_hotkeys.at(window);
}
void Window::keyCallback(GLFWwindow* w,int key, int scancode, int action, int mods) {

    auto&& io = ImGui::GetIO();
    if(!io.WantCaptureKeyboard) {
        s_hotkeys.at(w).press(key,mods,action);
    }
    ImGuiImpl::keyCallback(w,key,scancode,action,mods);
}


void Window::setSize(int w, int h) {
    glfwSetWindowSize(window,w,h);
}

std::array<int,2> Window::getSize() const {
    std::array<int,2> size;
    glfwGetWindowSize(window,&size[0],&size[1]);
    return size;
}

void Window::save_frame() {
    save_frame(get_frame_filename(m_frame_number++));
}
void Window::save_frame(const std::string& filename) {

#ifdef MTAO_HAS_LIBPNGPP
    auto [w,h] = getSize();
    std::vector<unsigned char> data(4*w*h);
    mtao::logging::info() << "Saving frame to disk(" << filename << "): " << w << "x" << h;
    //glReadBuffer(GL_FRONT);
    glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE, data.data());
    png::image<png::rgba_pixel> image(w,h);

    std::cout << "Writing to image" << std::endl;
    for (png::uint_32 y = 0; y < image.get_height(); ++y)
    {
        for (png::uint_32 x = 0; x < image.get_width(); ++x)
        {
            size_t o = 4*((h-1-y) * w+x);
            image[y][x] = png::rgba_pixel(
                    data[o+0],
                    data[o+1],
                    data[o+2],
                    data[o+3]
                    );
        }
    }

    image.write(filename);
#else
    logging::warn() << "No libpng++ so no screenshots!";
#endif
}
std::string Window::get_frame_filename(int frame) const {
    std::stringstream ss;
    ss << m_recording_prefix << std::setfill('0') << std::setw(6) << frame << ".png";
    return ss.str();
}
void Window::record(const std::function<bool(int)>& f, const std::string& prefix, bool show_gui) {
    m_recording_prefix = prefix;

    for(int idx = 0; f(idx); ++idx) {
        draw(show_gui);

        save_frame(get_frame_filename(idx));
    }
}
void Window::start_recording() { m_is_recording = true; }
void Window::stop_recording() { m_is_recording = false; }
void Window::set_recording_prefix(const std::string& str) { m_recording_prefix = str; }

void set_opengl_version_hints(int major, int minor, int profile) {
    if (!glfwInit()) {
        std::cerr <<" GLFWInit faillure!" << std::endl;
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, profile);
}

*/
}




