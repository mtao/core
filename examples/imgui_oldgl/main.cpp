#include "mtao/opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include <memory>
#include <algorithm>


using namespace mtao::opengl;



std::unique_ptr<Window> window;


ImVec4 clear_color = ImColor(114, 144, 154);

void gui_func() {
    {
        float look_min=0.0001f, look_max=100.0f;
        ImGui::Text("Hello, world!");
        float look_distance, rotation_angle;

        ImGui::SliderFloat("look_distance", &look_distance, look_min,look_max,"%.3f");
        ImGui::SliderFloat("angle", &rotation_angle,0,M_PI,"%.3f");




        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    }

}

void render(int width, int height) {
    // Rendering
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);





}



int main(int argc, char * argv[]) {

    set_opengl_version_hints(3,0,GLFW_OPENGL_ANY_PROFILE);
    window = std::make_unique<Window>();
    window->set_gui_func(gui_func);
    window->set_render_func(render);
    window->makeCurrent();

    window->run();

    exit(EXIT_SUCCESS);
}


