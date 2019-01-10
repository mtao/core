#ifndef IMGUI_IMPL_H
#define IMGUI_IMPL_H

#include "imgui.h"
struct GLFWwindow;
namespace mtao { namespace opengl {

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
