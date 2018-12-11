#ifndef WINDOW_H
#define WINDOW_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include "imgui_impl.h"
#include "mtao/hotkey_manager.hpp"
#include <array>

namespace mtao { namespace opengl {

class Window {
    public:
        Window(const std::string& name = "Name", int width = 640, int height = 480);
        ~Window();


        void draw(bool show_gui = true);
        void run();
        //int key, int scancode, int action, int mods
        void setKeyCallback(GLFWkeyfun f);
        void setMouseButtonCallback(GLFWkeyfun f);
        void setScrollCallback(GLFWscrollfun f);
        void setErrorCallback(GLFWerrorfun f);
        void makeCurrent();
        std::array<int,2> getSize() const;
        void setSize(int w, int h);
        void resize(int w, int h) { setSize(w,h); }


        void save_frame();
        void save_frame(const std::string& filename);
        void record(const std::function<bool(int)>& f, const std::string& prefix, bool show_gui = false);

        void set_render_func(const std::function<void(int,int)>& f) {m_render_func = f;}
        void set_gui_func(const std::function<void()>& f) {m_gui_func = f;}
        GLFWwindow* glfwWindow() {return window; }

        operator bool() const { return window; }
        ImGuiImpl& gui() { return m_gui; }
        const ImGuiImpl& gui() const { return m_gui; }
        static void keyCallback(GLFWwindow*,int key, int scancode, int action, int mods);
        HotkeyManager& hotkeys();
        const HotkeyManager& hotkeys() const;

        void start_recording();
        void stop_recording();
        bool is_recording() const { return m_is_recording; }
        void set_recording_prefix(const std::string& str);
        void reset_frame_number() { m_frame_number = 0; }

    private:
        std::string get_frame_filename(int frame) const;


    private:
        GLFWwindow* window;
        std::function<void()> m_gui_func;
        std::function<void(int,int)> m_render_func;
        ImGuiImpl m_gui;

        bool m_is_recording = false;
        std::string m_recording_prefix = "frame";
        int m_frame_number = 0;



        static std::map<GLFWwindow*,HotkeyManager> s_hotkeys;

        static size_t s_window_count;
};
void set_opengl_version_hints(int major=4, int minor=5, int profile=GLFW_OPENGL_CORE_PROFILE);

}}

#endif//WINDOW_H
