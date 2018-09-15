#include "mtao/opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include "mtao/opengl/shader.h"
#include "mtao/opengl/vao.h"
#include "mtao/opengl/bo.h"
#include <memory>
#include <algorithm>
#include "mtao/opengl/renderers/mesh.h"
#include "mtao/opengl/renderers/bbox.h"
#include "mtao/geometry/mesh/sphere.hpp"
#include "mtao/geometry/mesh/read_obj.hpp"
#include "mtao/geometry/bounding_box.hpp"
#include "mtao/opengl/camera.hpp"
#include <mtao/types.h>

#include <glm/gtc/matrix_transform.hpp> 

using namespace mtao::opengl;
Camera3D cam;


using ColVectors3f = mtao::ColVectors<GLfloat,3>;
using ColVectors3i = mtao::ColVectors<GLuint,3>;


bool animate = false;
bool save_frame = false;
std::unique_ptr<Window> window;
std::unique_ptr<renderers::MeshRenderer> renderer;
std::unique_ptr<renderers::BBoxRenderer3> bbox_renderer;
ImVec4 clear_color = ImColor(114, 144, 154);

void prepare_mesh(const ColVectors3f& V, const ColVectors3i&F) {
    mtao::logging::debug() << "preparing mesh. f range: " << F.minCoeff() << " << " << F.maxCoeff();
    mtao::logging::debug() << "preparing mesh. v count: " << V.cols();
    renderer = std::make_unique<renderers::MeshRenderer>(3);
    bbox_renderer = std::make_unique<renderers::BBoxRenderer3>();


    auto bb = mtao::geometry::bounding_box(V);

    float scale = bb.sizes().maxCoeff();

    ColVectors3f VV = V /  scale;

    renderer->setMesh(VV,F,true);
    {
    using BBox = renderers::BBoxRenderer3::BBox;
    BBox bb;
    for(int i = 0; i < VV.cols(); ++i) {
        bb.extend(VV.col(i));
    }
    //Artifact of how i normalize in my meshrenderer. should just delete that functionality
    auto m = ((bb.min() + bb.max())/2).eval();
    bb.min() -= m;
    bb.max() -= m;
    bbox_renderer->set(bb);
    }

    renderers::MeshRenderer::MatrixXgf C = renderer->computeNormals(V,F).array();
    renderer->setColor(C);
}
void set_mvp(int w, int h) {
    cam.set_shape(w,h);

    //cam.v() = glm::lookAt(glm::vec3(1,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0));
    cam.pan();
    cam.update();




}

void gui_func() {
    {

        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Checkbox("Animate",&animate);
        ImGui::Checkbox("Store",&save_frame);

        renderer->imgui_interface();
    }
    if(ImGui::Button("Reset  Camera?")) {
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
    bbox_renderer->set_mvp(cam.mvp());

    renderer->render();
    bbox_renderer->render();
    if(save_frame) {
        window->save_frame("frame.png");
        save_frame = false;
    }



}



int main(int argc, char * argv[]) {

    set_opengl_version_hint();
    window = std::make_unique<Window>();
    window->set_gui_func(gui_func);
    window->set_render_func(render);
    window->makeCurrent();

    mtao::ColVectors<GLfloat,3> V;
    mtao::ColVectors<int,3> F;
    if(argc < 2) {
        std::cout << "Need an obj input!" << std::endl;
        std::cout << "Loading a sphere mesh instead" << std::endl;
        std::tie(V,F) = mtao::geometry::mesh::sphere<GLfloat>(3);
    } else {
        std::tie(V,F) = mtao::geometry::mesh::read_objF(argv[1]);
    }
    prepare_mesh(V,F.cast<GLuint>());

    animate = true;
    /*
    window->record([&](int frame) -> bool {
            return frame < 20;
            }, "frame");
            */
    window->run();

    exit(EXIT_SUCCESS);
}


