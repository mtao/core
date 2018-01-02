#include "mtao/opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include "mtao/opengl/shader.h"
#include "mtao/opengl/vao.h"
#include "mtao/opengl/bo.h"
#include <memory>
#include "mesh.h"
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include "mtao/opengl/renderers/mesh.h"
#include "mtao/opengl/camera.hpp"

using namespace mtao::opengl;



float look_distance = 0.4;
glm::mat4 mvp_it;
float rotation_angle;
glm::vec3 edge_color;


std::unique_ptr<renderers::MeshRenderer> renderer;

std::unique_ptr<ShaderProgram> edge_program;
std::unique_ptr<Window> window;


Camera2D cam;

void set_mvp(int w, int h) {
    cam.set_shape(w,h);
    auto&& m = cam.m();
    m = glm::mat4();
    m = glm::rotate(m,(float) rotation_angle,glm::vec3(0,0,1));

    //cam.v() = glm::lookAt(glm::vec3(1,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0));
    cam.pan();
    cam.set_scale(look_distance);
    cam.update();

    mvp_it = cam.mvp_inv_trans();

}






void prepare_mesh(const Mesh& m) {
    renderer = std::make_unique<renderers::MeshRenderer>(2);
    renderer->setMesh(m.V,m.F,false);
}



ImVec4 clear_color = ImColor(114, 144, 154);

void gui_func() {
    {
        float look_min=0.0001f, look_max=100.0f;
        ImGui::Text("Hello, world!");

        ImGui::SliderFloat("look_distance", &look_distance, look_min,look_max,"%.3f");
        ImGui::SliderFloat("angle", &rotation_angle,0,M_PI,"%.3f");
        auto&& io = ImGui::GetIO();
        look_distance += .5 * io.MouseWheel;
        look_distance = std::min(std::max(look_distance,look_min),look_max);


        renderer->imgui_interface();


        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    }
    if(ImGui::Button("Reset  Translation?")) {
        cam.reset();
    }

}

void render(int width, int height) {
    // Rendering
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

    set_mvp(width,height);


    renderer->set_mvp(cam.mvp());
    renderer->set_mvp(cam.mv(),cam.p());
    renderer->render();
    auto&& io = ImGui::GetIO();
    auto p = cam.mouse_pos(io.MousePos);
    //mtao::logging::trace() << "Mouse coordinate: " << p.x << "," << p.y;


}



int main(int argc, char * argv[]) {

    if(argc <= 1) {
        std::cout << "Need an obj input!" << std::endl;
        return 1;
    }
    window = std::make_unique<Window>();
    window->set_gui_func(gui_func);
    window->set_render_func(render);
    window->makeCurrent();

    Mesh m(argv[1]);
    prepare_mesh(m);
    window->run();

    exit(EXIT_SUCCESS);
}


