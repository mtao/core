#include "opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include "opengl/shader.h"
#include "opengl/vao.h"
#include "opengl/bo.h"
#include <memory>
#include "mesh.h"
#include <algorithm>
#include "opengl/renderers/mesh.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

using namespace mtao::opengl;



float edge_threshold = 0.001;
float mean_edge_length = 1.0;
float look_distance = 0.4;
glm::vec3 face_color = glm::vec3(1,0,0);
glm::vec3 edge_color = glm::vec3(0,1,0);


std::unique_ptr<VAO> vertex_attribute;
std::unique_ptr<Window> window;


std::unique_ptr<renderers::MeshRenderer> renderer;


bool index_buffer_active = true;
bool use_barycentric_edges = true;

ImVec4 clear_color = ImColor(114, 144, 154);





void prepare_mesh(const Mesh& m) {
    renderer = std::make_unique<renderers::MeshRenderer>(3);
    renderer->setMesh(m.V,m.F);
}

void gui_func() {
    {
        float look_min=1.0f, look_max=5.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("edge_threshold", &edge_threshold, 0.0f, 0.01f,"%.5f");
        ImGui::SliderFloat("look_distance", &look_distance, look_min,look_max,"%.3f");
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::ColorEdit3("edge color", glm::value_ptr(edge_color));
        ImGui::ColorEdit3("face color", glm::value_ptr(face_color));
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        auto&& io = ImGui::GetIO();
        look_distance += .5 * io.MouseWheel;
        look_distance = std::min(std::max(look_distance,look_min),look_max);

        renderer->set_edge_color(edge_color);
        renderer->set_face_color(face_color);
        renderer->set_edge_threshold(edge_threshold);
    }

}

void render(int width, int height) {
    // Rendering
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    float ratio = width / (float) height;

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

    glm::mat4 m,v,mv,p,mvp;
    m = glm::rotate(m,(float) glfwGetTime() / 5.0f,glm::vec3(0,1,0));
    if(index_buffer_active) {
        p = glm::perspective(45.f,ratio,.1f,10.0f);
    } else {
        p = glm::ortho(-ratio,ratio,-1.f,1.f,1.f,-1.f);
    }

    v = glm::lookAt(glm::vec3(0,0,look_distance), glm::vec3(0,0,0), glm::vec3(0,1,0));
    mv = v*m;
    mvp = p * mv;


    renderer->set_mvp(mvp);
    renderer->render();



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
    vertex_attribute = std::make_unique<VAO>();
    vertex_attribute->bind();

    Mesh m(argv[1]);
    prepare_mesh(m);
    window->run();

    exit(EXIT_SUCCESS);
}


