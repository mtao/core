// GLAD/GLFW
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

#include <imgui.h>
#include "mtao/opengl/imgui_impl.h"

namespace mtao { namespace opengl {
    bool         ImGuiImpl::s_MousePressed[3] = { false, false, false };
    float        ImGuiImpl::s_MouseWheel = 0.0f;
void ImGuiImpl::setWindow(GLFWwindow* window) {
    m_Window = window;
}
ImGuiImpl::ImGuiImpl(GLFWwindow* window, bool use_old_gl): m_Window(window), m_use_old_gl(use_old_gl) {
    m_context = ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
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


}
ImGuiImpl::~ImGuiImpl() {
    invalidateDeviceObjects();
    //Theoretically I suspect I should be calling shutdown but it causes a segfault taht I don't understand
    //ImGui::Shutdown();
    ImGui::DestroyContext(m_context);
}
void ImGuiImpl::newFrame() {
    if (!m_FontTexture) {
        createDeviceObjects();
    }

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    glfwGetWindowSize(m_Window, &w, &h);
    glfwGetFramebufferSize(m_Window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup time step
    double current_time =  glfwGetTime();
    io.DeltaTime = m_Time > 0.0 ? (float)(current_time - m_Time) : (float)(1.0f/60.0f);
    m_Time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
    if (glfwGetWindowAttrib(m_Window, GLFW_FOCUSED))
    {
        double mouse_x, mouse_y;
        glfwGetCursorPos(m_Window, &mouse_x, &mouse_y);
        io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
    }
    else
    {
        io.MousePos = ImVec2(-1,-1);
    }

    for (int i = 0; i < 3; i++)
    {
        io.MouseDown[i] = s_MousePressed[i] || glfwGetMouseButton(m_Window, i) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        s_MousePressed[i] = false;
    }

    io.MouseWheel = s_MouseWheel;
    s_MouseWheel = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    glfwSetInputMode(m_Window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

    // Start the frame
    ImGui::NewFrame();
}
void ImGuiImpl::invalidateDeviceObjects() {
    if(!m_use_old_gl) {
        if (m_VaoHandle) glDeleteVertexArrays(1, &m_VaoHandle);
        if (m_VboHandle) glDeleteBuffers(1, &m_VboHandle);
        if (m_ElementsHandle) glDeleteBuffers(1, &m_ElementsHandle);
        m_VaoHandle = m_VboHandle = m_ElementsHandle = 0;

        if (m_ShaderHandle && m_VertHandle) glDetachShader(m_ShaderHandle, m_VertHandle);
        if (m_VertHandle) glDeleteShader(m_VertHandle);
        m_VertHandle = 0;

        if (m_ShaderHandle && m_FragHandle) glDetachShader(m_ShaderHandle, m_FragHandle);
        if (m_FragHandle) glDeleteShader(m_FragHandle);
        m_FragHandle = 0;

        if (m_ShaderHandle) glDeleteProgram(m_ShaderHandle);
        m_ShaderHandle = 0;
    }

    if (m_FontTexture)
    {
        glDeleteTextures(1, &m_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        m_FontTexture = 0;
    }
}
void ImGuiImpl::createDeviceObjects() {
    // Backup GL state
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_array_buffer, last_vertex_array;

    if(!m_use_old_gl) {
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

        const GLchar *vertex_shader =
            "#version 330\n"
            "uniform mat4 ProjMtx;\n"
            "in vec2 Position;\n"
            "in vec2 UV;\n"
            "in vec4 Color;\n"
            "out vec2 Fram_UV;\n"
            "out vec4 Fram_Color;\n"
            "void main()\n"
            "{\n"
            "	Fram_UV = UV;\n"
            "	Fram_Color = Color;\n"
            "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";

        const GLchar* fragment_shader =
            "#version 330\n"
            "uniform sampler2D Texture;\n"
            "in vec2 Fram_UV;\n"
            "in vec4 Fram_Color;\n"
            "out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "	Out_Color = Fram_Color * texture( Texture, Fram_UV.st);\n"
            "}\n";

        m_ShaderHandle = glCreateProgram();
        m_VertHandle = glCreateShader(GL_VERTEX_SHADER);
        m_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(m_VertHandle, 1, &vertex_shader, 0);
        glShaderSource(m_FragHandle, 1, &fragment_shader, 0);
        glCompileShader(m_VertHandle);
        glCompileShader(m_FragHandle);
        glAttachShader(m_ShaderHandle, m_VertHandle);
        glAttachShader(m_ShaderHandle, m_FragHandle);
        glLinkProgram(m_ShaderHandle);

        m_AttribLocationTex = glGetUniformLocation(m_ShaderHandle, "Texture");
        m_AttribLocationProjMtx = glGetUniformLocation(m_ShaderHandle, "ProjMtx");
        m_AttribLocationPosition = glGetAttribLocation(m_ShaderHandle, "Position");
        m_AttribLocationUV = glGetAttribLocation(m_ShaderHandle, "UV");
        m_AttribLocationColor = glGetAttribLocation(m_ShaderHandle, "Color");

        glGenBuffers(1, &m_VboHandle);
        glGenBuffers(1, &m_ElementsHandle);

        glGenVertexArrays(1, &m_VaoHandle);
        glBindVertexArray(m_VaoHandle);
        glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
        glEnableVertexAttribArray(m_AttribLocationPosition);
        glEnableVertexAttribArray(m_AttribLocationUV);
        glEnableVertexAttribArray(m_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
        glVertexAttribPointer(m_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(m_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(m_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF
    }

    createFontsTexture();

    glBindTexture(GL_TEXTURE_2D, last_texture);
    if(!m_use_old_gl) {
        // Restore modified GL state
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindVertexArray(last_vertex_array);
    }

}

void        ImGuiImpl::mouseButtonCallback(GLFWwindow*,int button, int action, int mods) {
    if (action == GLFW_PRESS && button >= 0 && button < 3)
        s_MousePressed[button] = true;
}
void        ImGuiImpl::scrollCallback(GLFWwindow*,double xoffset, double yoffset) {
    s_MouseWheel += (float)yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
}
void        ImGuiImpl::keyCallback(GLFWwindow*,int key, int scancode, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    (void)mods; // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}
void        ImGuiImpl::charCallback(GLFWwindow*,unsigned int c) {
    ImGuiIO& io = ImGui::GetIO();
    if (c > 0 && c < 0x10000)
        io.AddInputCharacter((unsigned short)c);
}
void ImGuiImpl::renderDrawLists2(ImDrawData* draw_data) {
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(draw_data->DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // We are using the OpenGL fixed pipeline to make the example code simpler to read!
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers, polygon fill.
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box); 
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_SCISSOR_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound

    // Setup viewport, orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single viewport apps.
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(draw_data->DisplayPos.x, draw_data->DisplayPos.x + draw_data->DisplaySize.x, draw_data->DisplayPos.y + draw_data->DisplaySize.y, draw_data->DisplayPos.y, -1.0f, +1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Render command lists
    ImVec2 pos = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, pos)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, uv)));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + IM_OFFSETOF(ImDrawVert, col)));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                // User callback (registered via ImDrawList::AddCallback)
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                ImVec4 clip_rect = ImVec4(pcmd->ClipRect.x - pos.x, pcmd->ClipRect.y - pos.y, pcmd->ClipRect.z - pos.x, pcmd->ClipRect.w - pos.y);
                if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                {
                    // Apply scissor/clipping rectangle
                    glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

                    // Bind texture, Draw
                    glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                    glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
                }
            }
            idx_buffer += pcmd->ElemCount;
        }
    }

    // Restore modified state
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, (GLuint)last_texture);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glPolygonMode(GL_FRONT, (GLenum)last_polygon_mode[0]); glPolygonMode(GL_BACK, (GLenum)last_polygon_mode[1]);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}
void ImGuiImpl::renderDrawLists3(ImDrawData* draw_data) {
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
    GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
    GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
    GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
    GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box); 
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUseProgram(m_ShaderHandle);
    glUniform1i(m_AttribLocationTex, 0);
    glUniformMatrix4fv(m_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(m_VaoHandle);

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    // Restore modified GL state
    glUseProgram(last_program);
    glActiveTexture(last_active_texture);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindVertexArray(last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFunc(last_blend_src, last_blend_dst);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}
const char* ImGuiImpl::getClipboardText(void* user_data) {
    return glfwGetClipboardString((GLFWwindow*)user_data);
}
void ImGuiImpl::setClipboardText(void* user_data, const char* text) {
    glfwSetClipboardString((GLFWwindow*)user_data, text);
}
bool ImGuiImpl::createFontsTexture() {
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &m_FontTexture);
    glBindTexture(GL_TEXTURE_2D, m_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)m_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}
void ImGuiImpl::render() {
    ImGui::Render();

    if(m_use_old_gl) {
    renderDrawLists2(ImGui::GetDrawData());
    } else {
    renderDrawLists3(ImGui::GetDrawData());
    }
}

}}
