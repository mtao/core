#ifndef IMGUI_IMPL_H
#define IMGUI_IMPL_H

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
    void renderDrawLists(void* draw_data);
    bool createFontsTexture();
    private:
    // Data
    GLFWwindow*  m_Window = nullptr;
    double       m_Time = 0.0f;
    static bool         s_MousePressed[3];
    static float        s_MouseWheel;
    GLuint       m_FontTexture = 0;
    int          m_ShaderHandle = 0, m_VertHandle = 0, m_FragHandle = 0;
    int          m_AttribLocationTex = 0, m_AttribLocationProjMtx = 0;
    int          m_AttribLocationPosition = 0, m_AttribLocationUV = 0, m_AttribLocationColor = 0;
    unsigned int m_VboHandle = 0, m_VaoHandle = 0, m_ElementsHandle = 0;
};

}}

#endif//IMGUI_IMPL_H
