#include "mtao/opengl/Window.h"
#include <Magnum/Math/Color.h>
#include <Magnum/GL/Renderer.h>


using namespace mtao::opengl;

using namespace Magnum;
using namespace Math::Literals;


class MeshViewer : public mtao::opengl::WindowBase {
  public:
    bool _showTestWindow = true;
    bool _showAnotherWindow = false;
    Color4 _clearColor = 0x72909aff_rgbaf;
    Float _floatValue = 0.0f;

    MeshViewer(const Arguments &args) : WindowBase(args) {
    }
    void gui() override {
        /* 1. Show a simple window.
Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appear in
a window called "Debug" automatically */
        {
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("Float", &_floatValue, 0.0f, 1.0f);
            if (ImGui::ColorEdit3("Clear Color", _clearColor.data()))
                GL::Renderer::setClearColor(_clearColor);
            if (ImGui::Button("Test Window"))
                _showTestWindow ^= true;
            if (ImGui::Button("Another Window"))
                _showAnotherWindow ^= true;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0 / Double(ImGui::GetIO().Framerate),
                        Double(ImGui::GetIO().Framerate));
        }

        /* 2. Show another simple window, now using an explicit Begin/End pair */
        if (_showAnotherWindow) {
            ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_FirstUseEver);
            ImGui::Begin("Another Window", &_showAnotherWindow);
            ImGui::Text("Hello");
            ImGui::End();
        }

        /* 3. Show the ImGui test window. Most of the sample code is in
               ImGui::ShowTestWindow() */
        if (_showTestWindow) {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
            ImGui::ShowTestWindow();
        }
    }
};


MAGNUM_APPLICATION_MAIN(MeshViewer)
