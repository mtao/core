#include "mtao/opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include "mtao/opengl/shader.h"
#include "mtao/opengl/vao.h"
#include "mtao/opengl/bo.h"
#include <memory>
#include "mesh.h"
#include <algorithm>
#include "mtao/opengl/renderers/mesh.h"
#include "mtao/geometry/mesh/sphere.hpp"
#include "mtao/geometry/bounding_box.hpp"
#include "mtao/opengl/camera.hpp"

#include <glm/gtc/matrix_transform.hpp> 

using namespace mtao::opengl;
Camera3D cam;



float look_distance = 0.4;
float rotation_angle = 1.0;
float rotation_angle2 = 0.5;
bool animate = false;
bool save_frame = false;
std::unique_ptr<Window> window;
std::unique_ptr<renderers::MeshRenderer> renderer;
ImVec4 clear_color = ImColor(114, 144, 154);

void prepare_mesh(const Mesh& m) {
    renderer = std::make_unique<renderers::MeshRenderer>(3);


    mtao::ColVectors<float,3> V = m.V;
    auto bb = mtao::geometry::bounding_box(V);

    float scale = bb.sizes().maxCoeff();

    V /= scale;

    renderer->setMesh(V,m.F,true);

    renderers::MeshRenderer::MatrixXgf C = renderer->computeNormals(m.V,m.F).array();
    renderer->setColor(C);
}
void set_mvp(int w, int h) {
    cam.set_shape(w,h);
    auto&& m = cam.m();
    m = glm::mat4();
    m = glm::rotate(m,(float) rotation_angle,glm::vec3(0,0,1));

    //cam.v() = glm::lookAt(glm::vec3(1,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0));
    cam.pan();
    cam.set_distance(look_distance);
    cam.update();




}

void gui_func() {
    {
        float look_min=1.0f, look_max=20.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("look_distance", &look_distance, look_min,look_max,"%.3f");
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        auto&& io = ImGui::GetIO();
        look_distance += .5 * io.MouseWheel;
        look_distance = std::min(std::max(look_distance,look_min),look_max);
        ImGui::Checkbox("Animate",&animate);
        ImGui::Checkbox("Store",&save_frame);
        ImGui::SliderFloat("angle", &rotation_angle,0,2*M_PI,"%.3f");
        ImGui::SliderFloat("angle2", &rotation_angle2,0,2*M_PI,"%.3f");

        renderer->imgui_interface();
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
    float ratio = width / (float) height;

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

    set_mvp(width,height);


    renderer->set_mvp(cam.mvp());
    renderer->set_mvp(cam.mv(),cam.p());
    renderer->render();
    if(save_frame) {
        window->save_frame("frame.png");
    save_frame = false;
    }



}



int main(int argc, char * argv[]) {

    window = std::make_unique<Window>();
    window->set_gui_func(gui_func);
    window->set_render_func(render);
    window->makeCurrent();

    if(argc < 2) {
        std::cout << "Need an obj input!" << std::endl;
        std::cout << "Loading a sphere mesh instead" << std::endl;


        mtao::ColVectors<GLfloat,3> V;
        mtao::ColVectors<int,3> F;
        std::tie(V,F) = mtao::geometry::mesh::sphere<GLfloat>(3);
        Mesh m(V,F.cast<unsigned int>());
        prepare_mesh(m);
    } else {
        Mesh m(argv[1]);
        prepare_mesh(m);
    }

    animate = true;
    window->record([&](int frame) -> bool {
            return frame < 20;
            }, "frame");
    window->run();

    exit(EXIT_SUCCESS);
}


