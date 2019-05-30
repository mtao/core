#ifndef IMGUI_IMPL_H
#define IMGUI_IMPL_H

#include "imgui.h"
struct GLFWwindow;
namespace mtao { namespace opengl {
    namespace imgui {
        IMGUI_API bool          DragDouble(const char* label, double* v, double v_speed = 1.0, double v_min = 0.0, double v_max = 0.0, const char* format = "%.6f", double power = 1.0);     // If v_min >= v_max we have no bound
        IMGUI_API bool          DragDouble2(const char* label, double v[2], double v_speed = 1.0, double v_min = 0.0, double v_max = 0.0, const char* format = "%.6f", double power = 1.0);
        IMGUI_API bool          DragDouble3(const char* label, double v[3], double v_speed = 1.0, double v_min = 0.0, double v_max = 0.0, const char* format = "%.6f", double power = 1.0);
        IMGUI_API bool          DragDouble4(const char* label, double v[4], double v_speed = 1.0, double v_min = 0.0, double v_max = 0.0, const char* format = "%.6f", double power = 1.0);
        IMGUI_API bool          SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format = "%.6f", double power = 1.0);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for power curve sliders
        IMGUI_API bool          SliderDouble2(const char* label, double v[2], double v_min, double v_max, const char* format = "%.6f", double power = 1.0);
        IMGUI_API bool          SliderDouble3(const char* label, double v[3], double v_min, double v_max, const char* format = "%.6f", double power = 1.0);
        IMGUI_API bool          SliderDouble4(const char* label, double v[4], double v_min, double v_max, const char* format = "%.6f", double power = 1.0);
        IMGUI_API bool          VSliderDouble(const char* label, const ImVec2& size, double* v, double v_min, double v_max, const char* format = "%.6f", double power = 1.0);
        IMGUI_API bool          InputDouble(const char* label, double* v, double step = 0.0, double step_fast = 0.0, const char* format = "%.6f", ImGuiInputTextFlags extra_flags = 0);
        IMGUI_API bool          InputDouble2(const char* label, double v[2], const char* format = "%.6f", ImGuiInputTextFlags extra_flags = 0);
        IMGUI_API bool          InputDouble3(const char* label, double v[3], const char* format = "%.6f", ImGuiInputTextFlags extra_flags = 0);
        IMGUI_API bool          InputDouble4(const char* label, double v[4], const char* format = "%.6f", ImGuiInputTextFlags extra_flags = 0);
    }

class ImGuiImpl {
    public:
    ImGuiImpl(GLFWwindow* window = nullptr);
    ~ImGuiImpl();
    void newFrame();

    void invalidateDeviceObjects();
    void createDeviceObjects();
    void        static mouseButtonCallback(GLFWwindow*,int button, int action, int mods);
    void        static scrollCallback(GLFWwindow*,double xoffset, double yoffset);
    void        static keyCallback(GLFWwindow*,int key, int scancode, int action, int mods);
    void        static charCallback(GLFWwindow*,unsigned int c);
    static const char* getClipboardText(void* user_data);
    static void setClipboardText(void* user_data, const char* text);
    void setWindow(GLFWwindow* window);
    void render();

    private:
    void renderDrawLists(ImDrawData* draw_data);
    bool createFontsTexture();
    static bool CheckShader(GLuint handle, const char* desc);
    static bool CheckProgram(GLuint handle, const char* desc);
    void updateMouseCursor();
    void updateMousePosAndButtons();
    private:
    // Data
    GLFWwindow*  m_Window = nullptr;
    ImGuiContext* m_context = nullptr;


    double           m_Time = 0.0;
    static bool             s_MouseJustPressed[5];
    static GLFWcursor*      s_MouseCursors[ImGuiMouseCursor_COUNT];



    char         m_GlslVersionString[32] = "";
    GLuint       m_FontTexture = 0;
    GLuint       m_ShaderHandle = 0, m_VertHandle = 0, m_FragHandle = 0;
    int          m_AttribLocationTex = 0, m_AttribLocationProjMtx = 0;
    int          m_AttribLocationPosition = 0, m_AttribLocationUV = 0, m_AttribLocationColor = 0;
    unsigned int m_VboHandle = 0, m_ElementsHandle = 0;

};

}}

#endif//IMGUI_IMPL_H
