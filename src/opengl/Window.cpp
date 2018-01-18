#include "mtao/opengl/Window.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <imgui.h>
#include <vector>
#include <png++/png.hpp>
#include <iomanip>
#include <mtao/logging/logger.hpp>

namespace mtao {namespace opengl {
size_t Window::s_window_count = 0;
std::map<GLFWwindow*,HotkeyManager> Window::s_hotkeys;

static void error_callback(int error, const char* description)
{
    std::stringstream ss;
    ss << "Error: " << description << std::endl;
    throw std::runtime_error(ss.str());
}

/*
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
*/

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



   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("GLFW window creation failed!");
    }
    s_hotkeys[window];
    m_gui.setWindow(window);
    makeCurrent();
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    printGLInfo();


    glfwSetMouseButtonCallback(window, ImGuiImpl::mouseButtonCallback);
    glfwSetScrollCallback(window, ImGuiImpl::scrollCallback);
    glfwSetKeyCallback(window,    Window::keyCallback);
    glfwSetCharCallback(window,   ImGuiImpl::charCallback);


    setErrorCallback(error_callback);
    glfwSwapInterval(1);
}

Window::~Window() {
    s_window_count--;
    if(window) {
        glfwDestroyWindow(window);
    }
    if(s_window_count == 0) {
        glfwTerminate();
    }
}


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
    m_gui.newFrame();
    m_gui_func();
    m_render_func(display_w,display_h);
    if(show_gui) {
    m_gui.render();
    }
    glfwSwapBuffers(window);

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
    s_hotkeys.at(w).press(key,mods,action);
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

void Window::save_frame(const std::string& filename) {

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
}
void Window::record(const std::function<bool(int)>& f, const std::string& prefix, bool show_gui) {

    for(int idx = 0; f(idx); ++idx) {
        draw(show_gui);

        std::stringstream ss;
        ss << prefix << std::setfill('0') << std::setw(6) << idx << ".png";
        save_frame(ss.str());
    }
}

}}
