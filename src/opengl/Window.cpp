#include "opengl/Window.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <imgui.h>

namespace mtao {namespace opengl {
size_t Window::s_window_count = 0;

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
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;;

    std::cout << "OpenGL Renderer: " <<  glGetString(GL_RENDERER) << std::endl;;
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
    m_gui.setWindow(window);
    makeCurrent();
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    printGLInfo();


    glfwSetMouseButtonCallback(window, ImGuiImpl::mouseButtonCallback);
    glfwSetScrollCallback(window, ImGuiImpl::scrollCallback);
    glfwSetKeyCallback(window,    ImGuiImpl::keyCallback);
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

void Window::draw() {
    //ImGuiIO& io = ImGui::GetIO();
    makeCurrent();
    glfwPollEvents();

    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"

    int display_w, display_h;
    glfwGetFramebufferSize(window,&display_w, &display_h);
    m_render_func(display_w,display_h);
    m_gui.newFrame();
    m_gui_func();
    m_gui.render();
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


}}
