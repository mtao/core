// GLAD/GLFW
#include "mtao/opengl/opengl_loader.hpp"
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif
#include <mtao/logging/logger.hpp>
using namespace mtao::logging;

#include <imgui.h>
#include "mtao/opengl/imgui_impl.h"
#ifdef USE_IMGUI_IMPL
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#endif
#include <stdio.h>

namespace mtao {
namespace opengl {
    bool ImGuiImpl::s_MouseJustPressed[5] = { false, false, false, false, false };
    GLFWcursor *ImGuiImpl::s_MouseCursors[ImGuiMouseCursor_COUNT] = { 0 };
    void ImGuiImpl::setWindow(GLFWwindow *window) {
        m_Window = window;

        if (window != nullptr) {
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            const char *glsl_version = "#version 410";
            ImGui_ImplOpenGL3_Init(glsl_version);
            ImGui::StyleColorsDark();
            return;
        }
    }
    ImGuiImpl::ImGuiImpl(GLFWwindow *window) : m_Window(window) {
#ifdef USE_IMGUI_IMPL
        // Setup Dear ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls


        setWindow(window);

#else
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

        io.SetClipboardTextFn = ImGuiImpl::setClipboardText;
        io.GetClipboardTextFn = ImGuiImpl::getClipboardText;
        io.ClipboardUserData = m_Window;
#ifdef _WIN32
        io.ImeWindowHandle = glfwGetWin32Window(m_Window);
#endif
#endif
    }
    ImGuiImpl::~ImGuiImpl() {
#ifdef USE_IMGUI_IMPL
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
#else
        invalidateDeviceObjects();

        //Theoretically I suspect I should be calling shutdown but it causes a segfault taht I don't understand
        //ImGui::Shutdown();
        ImGui::DestroyContext(m_context);
#endif
    }

    void ImGuiImpl::updateMousePosAndButtons() {
        // Update buttons
        ImGuiIO &io = ImGui::GetIO();
        for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) {
            // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
            io.MouseDown[i] = s_MouseJustPressed[i] || glfwGetMouseButton(m_Window, i) != 0;
            s_MouseJustPressed[i] = false;
        }

        // Update mouse position
        const ImVec2 mouse_pos_backup = io.MousePos;
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
#ifdef __EMSCRIPTEN__
        const bool focused = true;// Emscripten
#else
        const bool focused = glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) != 0;
#endif
        if (focused) {
            if (io.WantSetMousePos) {
                glfwSetCursorPos(m_Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
            } else {
                double mouse_x, mouse_y;
                glfwGetCursorPos(m_Window, &mouse_x, &mouse_y);
                io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
            }
        }
    }

    void ImGuiImpl::updateMouseCursor() {
        ImGuiIO &io = ImGui::GetIO();
        if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
            return;

        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        } else {
            // Show OS mouse cursor
            // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
            glfwSetCursor(m_Window, s_MouseCursors[imgui_cursor] ? s_MouseCursors[imgui_cursor] : s_MouseCursors[ImGuiMouseCursor_Arrow]);
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    void ImGuiImpl::newFrame() {
#ifdef USE_IMGUI_IMPL
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#else
        if (!m_FontTexture) {
            createDeviceObjects();
        }

        ImGuiIO &io = ImGui::GetIO();
        IM_ASSERT(io.Fonts->IsBuilt());// Font atlas needs to be built, call renderer _NewFrame() function e.g. ImGui_ImplOpenGL3_NewFrame()

        // Setup display size
        int w, h;
        int display_w, display_h;
        glfwGetWindowSize(m_Window, &w, &h);
        glfwGetFramebufferSize(m_Window, &display_w, &display_h);
        io.DisplaySize = ImVec2((float)w, (float)h);
        io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

        // Setup time step
        double current_time = glfwGetTime();
        io.DeltaTime = m_Time > 0.0 ? (float)(current_time - m_Time) : (float)(1.0f / 60.0f);
        m_Time = current_time;

        updateMousePosAndButtons();
        updateMouseCursor();

        // Gamepad navigation mapping [BETA]
        memset(io.NavInputs, 0, sizeof(io.NavInputs));
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) {
            // Update gamepad inputs
#define MAP_BUTTON(NAV_NO, BUTTON_NO)                                                                   \
    {                                                                                                   \
        if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) io.NavInputs[NAV_NO] = 1.0f; \
    }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1)                     \
    {                                                           \
        float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0;  \
        v = (v - V0) / (V1 - V0);                               \
        if (v > 1.0f) v = 1.0f;                                 \
        if (io.NavInputs[NAV_NO] < v) io.NavInputs[NAV_NO] = v; \
    }
            int axes_count = 0, buttons_count = 0;
            const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
            const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
            MAP_BUTTON(ImGuiNavInput_Activate, 0);// Cross / A
            MAP_BUTTON(ImGuiNavInput_Cancel, 1);// Circle / B
            MAP_BUTTON(ImGuiNavInput_Menu, 2);// Square / X
            MAP_BUTTON(ImGuiNavInput_Input, 3);// Triangle / Y
            MAP_BUTTON(ImGuiNavInput_DpadLeft, 13);// D-Pad Left
            MAP_BUTTON(ImGuiNavInput_DpadRight, 11);// D-Pad Right
            MAP_BUTTON(ImGuiNavInput_DpadUp, 10);// D-Pad Up
            MAP_BUTTON(ImGuiNavInput_DpadDown, 12);// D-Pad Down
            MAP_BUTTON(ImGuiNavInput_FocusPrev, 4);// L1 / LB
            MAP_BUTTON(ImGuiNavInput_FocusNext, 5);// R1 / RB
            MAP_BUTTON(ImGuiNavInput_TweakSlow, 4);// L1 / LB
            MAP_BUTTON(ImGuiNavInput_TweakFast, 5);// R1 / RB
            MAP_ANALOG(ImGuiNavInput_LStickLeft, 0, -0.3f, -0.9f);
            MAP_ANALOG(ImGuiNavInput_LStickRight, 0, +0.3f, +0.9f);
            MAP_ANALOG(ImGuiNavInput_LStickUp, 1, +0.3f, +0.9f);
            MAP_ANALOG(ImGuiNavInput_LStickDown, 1, -0.3f, -0.9f);
#undef MAP_BUTTON
#undef MAP_ANALOG
            if (axes_count > 0 && buttons_count > 0)
                io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
            else
                io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
        }
        ImGui::NewFrame();
#endif
    }
    void ImGuiImpl::invalidateDeviceObjects() {
        if (m_VboHandle) glDeleteBuffers(1, &m_VboHandle);
        if (m_ElementsHandle) glDeleteBuffers(1, &m_ElementsHandle);
        m_VboHandle = m_ElementsHandle = 0;

        if (m_ShaderHandle && m_VertHandle) glDetachShader(m_ShaderHandle, m_VertHandle);
        if (m_VertHandle) glDeleteShader(m_VertHandle);
        m_VertHandle = 0;

        if (m_ShaderHandle && m_FragHandle) glDetachShader(m_ShaderHandle, m_FragHandle);
        if (m_FragHandle) glDeleteShader(m_FragHandle);
        m_FragHandle = 0;

        if (m_ShaderHandle) glDeleteProgram(m_ShaderHandle);
        m_ShaderHandle = 0;

        if (m_FontTexture) {
            ImGuiIO &io = ImGui::GetIO();
            glDeleteTextures(1, &m_FontTexture);
            io.Fonts->TexID = 0;
            m_FontTexture = 0;
        }
    }
    void ImGuiImpl::createDeviceObjects() {
#ifdef USE_IMGUI_IMPL
        // Parse GLSL version string
        int glsl_version = 130;
        sscanf(m_GlslVersionString, "#version %d", &glsl_version);

#else
        // Backup GL state
        // Backup GL state
        GLint last_texture, last_array_buffer, last_vertex_array;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);


        const GLchar *vertex_shader_glsl_120 =
          "uniform mat4 ProjMtx;\n"
          "attribute vec2 Position;\n"
          "attribute vec2 UV;\n"
          "attribute vec4 Color;\n"
          "varying vec2 Frag_UV;\n"
          "varying vec4 Frag_Color;\n"
          "void main()\n"
          "{\n"
          "    Frag_UV = UV;\n"
          "    Frag_Color = Color;\n"
          "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
          "}\n";

        const GLchar *vertex_shader_glsl_130 =
          "uniform mat4 ProjMtx;\n"
          "in vec2 Position;\n"
          "in vec2 UV;\n"
          "in vec4 Color;\n"
          "varying vec2 Frag_UV;\n"
          "varying vec4 Frag_Color;\n"
          "void main()\n"
          "{\n"
          "    Frag_UV = UV;\n"
          "    Frag_Color = Color;\n"
          "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
          "}\n";

        const GLchar *vertex_shader_glsl_300_es =
          "precision mediump float;\n"
          "layout (location = 0) in vec2 Position;\n"
          "layout (location = 1) in vec2 UV;\n"
          "layout (location = 2) in vec4 Color;\n"
          "uniform mat4 ProjMtx;\n"
          "out vec2 Frag_UV;\n"
          "out vec4 Frag_Color;\n"
          "void main()\n"
          "{\n"
          "    Frag_UV = UV;\n"
          "    Frag_Color = Color;\n"
          "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
          "}\n";

        const GLchar *vertex_shader_glsl_410_core =
          "layout (location = 0) in vec2 Position;\n"
          "layout (location = 1) in vec2 UV;\n"
          "layout (location = 2) in vec4 Color;\n"
          "uniform mat4 ProjMtx;\n"
          "out vec2 Frag_UV;\n"
          "out vec4 Frag_Color;\n"
          "void main()\n"
          "{\n"
          "    Frag_UV = UV;\n"
          "    Frag_Color = Color;\n"
          "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
          "}\n";

        const GLchar *fragment_shader_glsl_120 =
          "#ifdef GL_ES\n"
          "    precision mediump float;\n"
          "#endif\n"
          "uniform sampler2D Texture;\n"
          "varying vec2 Frag_UV;\n"
          "varying vec4 Frag_Color;\n"
          "void main()\n"
          "{\n"
          "    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);\n"
          "}\n";

        const GLchar *fragment_shader_glsl_130 =
          "uniform sampler2D Texture;\n"
          "varying vec2 Frag_UV;\n"
          "varying vec4 Frag_Color;\n"
          "void main()\n"
          "{\n"
          "    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);\n"
          "}\n";

        const GLchar *fragment_shader_glsl_300_es =
          "precision mediump float;\n"
          "uniform sampler2D Texture;\n"
          "in vec2 Frag_UV;\n"
          "in vec4 Frag_Color;\n"
          "layout (location = 0) out vec4 Out_Color;\n"
          "void main()\n"
          "{\n"
          "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
          "}\n";

        const GLchar *fragment_shader_glsl_410_core =
          "in vec2 Frag_UV;\n"
          "in vec4 Frag_Color;\n"
          "uniform sampler2D Texture;\n"
          "layout (location = 0) out vec4 Out_Color;\n"
          "void main()\n"
          "{\n"
          "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
          "}\n";

        // Select shaders matching our GLSL versions
        const GLchar *vertex_shader = NULL;
        const GLchar *fragment_shader = NULL;
        if (glsl_version < 130) {
            vertex_shader = vertex_shader_glsl_120;
            fragment_shader = fragment_shader_glsl_120;
        } else if (glsl_version == 410) {
            vertex_shader = vertex_shader_glsl_410_core;
            fragment_shader = fragment_shader_glsl_410_core;
        } else if (glsl_version == 300) {
            vertex_shader = vertex_shader_glsl_300_es;
            fragment_shader = fragment_shader_glsl_300_es;
        } else {
            vertex_shader = vertex_shader_glsl_130;
            fragment_shader = fragment_shader_glsl_130;
        }

        // Create shaders
        const GLchar *vertex_shader_with_version[2] = { m_GlslVersionString, vertex_shader };
        m_VertHandle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(m_VertHandle, 2, vertex_shader_with_version, NULL);
        glCompileShader(m_VertHandle);
        CheckShader(m_VertHandle, "vertex shader");

        const GLchar *fragment_shader_with_version[2] = { m_GlslVersionString, fragment_shader };
        m_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(m_FragHandle, 2, fragment_shader_with_version, NULL);
        glCompileShader(m_FragHandle);
        CheckShader(m_FragHandle, "fragment shader");

        m_ShaderHandle = glCreateProgram();
        glAttachShader(m_ShaderHandle, m_VertHandle);
        glAttachShader(m_ShaderHandle, m_FragHandle);
        glLinkProgram(m_ShaderHandle);
        CheckProgram(m_ShaderHandle, "shader program");

        m_AttribLocationTex = glGetUniformLocation(m_ShaderHandle, "Texture");
        m_AttribLocationProjMtx = glGetUniformLocation(m_ShaderHandle, "ProjMtx");
        m_AttribLocationPosition = glGetAttribLocation(m_ShaderHandle, "Position");
        m_AttribLocationUV = glGetAttribLocation(m_ShaderHandle, "UV");
        m_AttribLocationColor = glGetAttribLocation(m_ShaderHandle, "Color");

        // Create buffers
        glGenBuffers(1, &m_VboHandle);
        glGenBuffers(1, &m_ElementsHandle);

        createFontsTexture();

        // Restore modified GL state
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindVertexArray(last_vertex_array);
#endif
    }

    void ImGuiImpl::mouseButtonCallback(GLFWwindow *, int button, int action, int mods) {
        if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(s_MouseJustPressed))
            s_MouseJustPressed[button] = true;
    }
    void ImGuiImpl::scrollCallback(GLFWwindow *, double xoffset, double yoffset) {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseWheelH += (float)xoffset;
        io.MouseWheel += (float)yoffset;
    }
    void ImGuiImpl::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
        ImGuiIO &io = ImGui::GetIO();
        if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
        if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;

        (void)mods;// Modifiers are not reliable across systems
        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
    }
    void ImGuiImpl::charCallback(GLFWwindow *, unsigned int c) {
        ImGuiIO &io = ImGui::GetIO();
        if (c > 0 && c < 0x10000)
            io.AddInputCharacter((unsigned short)c);
    }
    void ImGuiImpl::renderDrawLists(ImDrawData *draw_data) {
        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        ImGuiIO &io = ImGui::GetIO();
        int fb_width = (int)(draw_data->DisplaySize.x * io.DisplayFramebufferScale.x);
        int fb_height = (int)(draw_data->DisplaySize.y * io.DisplayFramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;
        draw_data->ScaleClipRects(io.DisplayFramebufferScale);

        // Backup GL state
        GLenum last_active_texture;
        glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&last_active_texture);
        glActiveTexture(GL_TEXTURE0);
        GLint last_program;
        glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
#ifdef GL_SAMPLER_BINDING
        GLint last_sampler;
        glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
#endif
        GLint last_array_buffer;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        GLint last_vertex_array;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
#ifdef GL_POLYGON_MODE
        GLint last_polygon_mode[2];
        glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
        GLint last_viewport[4];
        glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLint last_scissor_box[4];
        glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
        GLenum last_blend_src_rgb;
        glGetIntegerv(GL_BLEND_SRC_RGB, (GLint *)&last_blend_src_rgb);
        GLenum last_blend_dst_rgb;
        glGetIntegerv(GL_BLEND_DST_RGB, (GLint *)&last_blend_dst_rgb);
        GLenum last_blend_src_alpha;
        glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint *)&last_blend_src_alpha);
        GLenum last_blend_dst_alpha;
        glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint *)&last_blend_dst_alpha);
        GLenum last_blend_equation_rgb;
        glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint *)&last_blend_equation_rgb);
        GLenum last_blend_equation_alpha;
        glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint *)&last_blend_equation_alpha);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

        // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

        // Setup viewport, orthographic projection matrix
        // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single viewport apps.
        glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        const float ortho_projection[4][4] = {
            { 2.0f / (R - L), 0.0f, 0.0f, 0.0f },
            { 0.0f, 2.0f / (T - B), 0.0f, 0.0f },
            { 0.0f, 0.0f, -1.0f, 0.0f },
            { (R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f },
        };
        glUseProgram(m_ShaderHandle);
        glUniform1i(m_AttribLocationTex, 0);
        glUniformMatrix4fv(m_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
#ifdef GL_SAMPLER_BINDING
        glBindSampler(0, 0);// We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.
#endif
        // Recreate the VAO every time
        // (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
        GLuint vao_handle = 0;
        glGenVertexArrays(1, &vao_handle);
        glBindVertexArray(vao_handle);
        glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
        glEnableVertexAttribArray(m_AttribLocationPosition);
        glEnableVertexAttribArray(m_AttribLocationUV);
        glEnableVertexAttribArray(m_AttribLocationColor);
        glVertexAttribPointer(m_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid *)IM_OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(m_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid *)IM_OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(m_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid *)IM_OFFSETOF(ImDrawVert, col));

        // Draw
        ImVec2 pos = draw_data->DisplayPos;
        for (int n = 0; n < draw_data->CmdListsCount; n++) {
            const ImDrawList *cmd_list = draw_data->CmdLists[n];
            const ImDrawIdx *idx_buffer_offset = 0;

            glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid *)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementsHandle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid *)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
                const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback) {
                    // User callback (registered via ImDrawList::AddCallback)
                    pcmd->UserCallback(cmd_list, pcmd);
                } else {
                    ImVec4 clip_rect = ImVec4(pcmd->ClipRect.x - pos.x, pcmd->ClipRect.y - pos.y, pcmd->ClipRect.z - pos.x, pcmd->ClipRect.w - pos.y);
                    if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f) {
                        // Apply scissor/clipping rectangle
                        glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

                        // Bind texture, Draw
                        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                        glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
                    }
                }
                idx_buffer_offset += pcmd->ElemCount;
            }
        }
        glDeleteVertexArrays(1, &vao_handle);

        // Restore modified GL state
        glUseProgram(last_program);
        glBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef GL_SAMPLER_BINDING
        glBindSampler(0, last_sampler);
#endif
        glActiveTexture(last_active_texture);
        glBindVertexArray(last_vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
        if (last_enable_blend)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
        if (last_enable_cull_face)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
        if (last_enable_depth_test)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        if (last_enable_scissor_test)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
        glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
        glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
        glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
    }
    void ImGuiImpl::render() {
        ImGui::Render();

#ifdef USE_IMGUI_IMPL
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
        renderDrawLists(ImGui::GetDrawData());
#endif
    }
    // If you get an error please report on github. You may try different GL context version or GLSL version.
    bool ImGuiImpl::CheckShader(GLuint handle, const char *desc) {
        GLint status = 0, log_length = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if (status == GL_FALSE)
            fprintf(stderr, "ERROR: ImGuiImpl::CreateDeviceObjects: failed to compile %s!\n", desc);
        if (log_length > 0) {
            ImVector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetShaderInfoLog(handle, log_length, NULL, (GLchar *)buf.begin());
            fprintf(stderr, "%s\n", buf.begin());
        }
        return status == GL_TRUE;
    }

    // If you get an error please report on github. You may try different GL context version or GLSL version.
    bool ImGuiImpl::CheckProgram(GLuint handle, const char *desc) {
        GLint status = 0, log_length = 0;
        glGetProgramiv(handle, GL_LINK_STATUS, &status);
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if (status == GL_FALSE)
            fprintf(stderr, "ERROR: ImGuiImpil::CreateDeviceObjects: failed to link %s!\n", desc);
        if (log_length > 0) {
            ImVector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetProgramInfoLog(handle, log_length, NULL, (GLchar *)buf.begin());
            fprintf(stderr, "%s\n", buf.begin());
        }
        return status == GL_TRUE;
    }


    const char *ImGuiImpl::getClipboardText(void *user_data) {
        return glfwGetClipboardString((GLFWwindow *)user_data);
    }
    void ImGuiImpl::setClipboardText(void *user_data, const char *text) {
        glfwSetClipboardString((GLFWwindow *)user_data, text);
    }
    bool ImGuiImpl::createFontsTexture() {
        // Build texture atlas
        ImGuiIO &io = ImGui::GetIO();
        unsigned char *pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);// Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

        // Upload texture to graphics system
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &m_FontTexture);
        glBindTexture(GL_TEXTURE_2D, m_FontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        // Store our identifier
        io.Fonts->TexID = (ImTextureID)(intptr_t)m_FontTexture;

        // Restore state
        glBindTexture(GL_TEXTURE_2D, last_texture);

        return true;
    }

    namespace imgui {
        bool DragDouble(const char *label, double *v, double v_speed, double v_min, double v_max, const char *format, double power) {
            return ImGui::DragScalar(label, ImGuiDataType_Double, v, v_speed, &v_min, &v_max, format, power);
        }

        bool DragDouble2(const char *label, double v[2], double v_speed, double v_min, double v_max, const char *format, double power) {
            return ImGui::DragScalarN(label, ImGuiDataType_Double, v, 2, v_speed, &v_min, &v_max, format, power);
        }

        bool DragDouble3(const char *label, double v[3], double v_speed, double v_min, double v_max, const char *format, double power) {
            return ImGui::DragScalarN(label, ImGuiDataType_Double, v, 3, v_speed, &v_min, &v_max, format, power);
        }

        bool DragDouble4(const char *label, double v[4], double v_speed, double v_min, double v_max, const char *format, double power) {
            return ImGui::DragScalarN(label, ImGuiDataType_Double, v, 4, v_speed, &v_min, &v_max, format, power);
        }
        bool SliderDouble(const char *label, double *v, double v_min, double v_max, const char *format, double power) {
            return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, power);
        }

        bool SliderDouble2(const char *label, double v[2], double v_min, double v_max, const char *format, double power) {
            return ImGui::SliderScalarN(label, ImGuiDataType_Double, v, 2, &v_min, &v_max, format, power);
        }

        bool SliderDouble3(const char *label, double v[3], double v_min, double v_max, const char *format, double power) {
            return ImGui::SliderScalarN(label, ImGuiDataType_Double, v, 3, &v_min, &v_max, format, power);
        }

        bool SliderDouble4(const char *label, double v[4], double v_min, double v_max, const char *format, double power) {
            return ImGui::SliderScalarN(label, ImGuiDataType_Double, v, 4, &v_min, &v_max, format, power);
        }

        bool VSliderDouble(const char *label, const ImVec2 &size, double *v, double v_min, double v_max, const char *format, double power) {
            return ImGui::VSliderScalar(label, size, ImGuiDataType_Double, v, &v_min, &v_max, format, power);
        }
        bool InputDouble(const char *label, double *v, double step, double step_fast, const char *format, ImGuiInputTextFlags extra_flags) {
            extra_flags |= ImGuiInputTextFlags_CharsScientific;
            return ImGui::InputScalar(label, ImGuiDataType_Double, (void *)v, (void *)(step > 0.0f ? &step : NULL), (void *)(step_fast > 0.0f ? &step_fast : NULL), format, extra_flags);
        }

        bool InputDouble2(const char *label, double v[2], const char *format, ImGuiInputTextFlags extra_flags) {
            return ImGui::InputScalarN(label, ImGuiDataType_Double, v, 2, NULL, NULL, format, extra_flags);
        }

        bool InputDouble3(const char *label, double v[3], const char *format, ImGuiInputTextFlags extra_flags) {
            return ImGui::InputScalarN(label, ImGuiDataType_Double, v, 3, NULL, NULL, format, extra_flags);
        }

        bool InputDouble4(const char *label, double v[4], const char *format, ImGuiInputTextFlags extra_flags) {
            return ImGui::InputScalarN(label, ImGuiDataType_Double, v, 4, NULL, NULL, format, extra_flags);
        }

    }// namespace imgui


}// namespace opengl
}// namespace mtao
