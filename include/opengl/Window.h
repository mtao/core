#ifndef WINDOW_H
#define WINDOW_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include "imgui_impl.h"

namespace mtao { namespace opengl {

class Window {
    public:
        Window(const std::string& name = "Name", int width = 640, int height = 480);
        ~Window();

        void draw();
        void run();
        //int key, int scancode, int action, int mods
        void setKeyCallback(GLFWkeyfun f);
        void setMouseButtonCallback(GLFWkeyfun f);
        void setScrollCallback(GLFWscrollfun f);
        void setErrorCallback(GLFWerrorfun f);
        void makeCurrent();

        void set_render_func(const std::function<void(int,int)>& f) {m_render_func = f;}
        void set_gui_func(const std::function<void()>& f) {m_gui_func = f;}
        GLFWwindow* glfwWindow() {return window; }

        operator bool() const { return window; }
    private:
        GLFWwindow* window;
        std::function<void()> m_gui_func;
        std::function<void(int,int)> m_render_func;
        ImGuiImpl m_gui;


        static size_t s_window_count;
};

}}

#endif//WINDOW_H
