#include <mtao/types.hpp>
#include "mtao/opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include "mtao/opengl/shader.h"
#include "mtao/opengl/renderers/bbox.h"
#include "mtao/opengl/renderers/axis.h"
#include <mtao/eigen/stack.h>
#include <memory>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include "mtao/opengl/renderers/mesh.h"
#include "mtao/opengl/camera.hpp"
#include <mtao/eigen_utils.h>
#include <mtao/logging/logger.hpp>
#include <mtao/geometry/grid/triangulation.hpp>
using namespace mtao::logging;

using namespace mtao::opengl;

std::vector<glm::vec3> pts;

enum class Mode: int { Smoothing, LSReinitialization };
Mode mode = Mode::LSReinitialization;

float permeability = 100.0;
float timestep = 1000.0;
bool animate = false;
using Vec = mtao::VectorX<GLfloat>;
Vec data;
Vec data_original;
Vec dx;
Vec dy;
Vec signs;
Eigen::SparseMatrix<float> L;

int NI=20;
int NJ=20;
int NK=20;
Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;

std::unique_ptr<renderers::MeshRenderer> renderer;
std::unique_ptr<renderers::BBoxRenderer3> bbox_renderer;
//glm::dvec4 clip_plane_equation;

std::unique_ptr<Window> window;


Camera3D cam;

void set_mvp(int w, int h) {
    cam.set_shape(w,h);
    cam.pan();
    cam.update();
}






void prepare_mesh(int i, int j, int k) {

    renderer = std::make_unique<renderers::MeshRenderer>(3);
    bbox_renderer = std::make_unique<renderers::BBoxRenderer3>();

    debug() << "Starting to make mesh";
    mtao::geometry::grid::Grid3f g(std::array<int,3>{{i,j,k}});
    mtao::geometry::grid::GridTriangulator<decltype(g)> gt(g);
    auto V = gt.vertices();
    auto F = gt.faces();
    V.array() -= .5;

    //mtao::RowVectorX<GLfloat> col = V.colwise().norm().array() - GLfloat(.25);
    //data = Eigen::Map<Mat>(col.data(),i,j);
    //
    data = V.colwise().norm().transpose().array() - .3 ;


    debug() << "Done making mesh";

    using BBox = renderers::BBoxRenderer3::BBox;
    BBox bb;
    for(int i = 0; i < V.cols(); ++i) {
        bb.extend(V.col(i));
    }
    debug() << "Done making bb";
    bbox_renderer->set(bb);

    /*
    data = data.array().pow(3);
    */
    //data.setZero();
    //data(NI/2,NJ/2) = 1;
    //data(NI/3,NJ/2) = -1;


    renderer->setVertices(V,false);
    renderer->setMesh(V,F,false);
    renderer->setEdges(gt.edges());
    renderer->set_edge_style(renderers::MeshRenderer::EdgeStyle::Mesh);
    renderer->set_vertex_style();
    renderer->set_face_style();

    auto col = data.transpose();
    mtao::MatrixX<GLfloat> Col = mtao::eigen::vstack((col.array()>0).select(col,0),-(col.array()<0).select(col,0),mtao::RowVectorX<GLfloat>::Zero(data.size()));
    renderer->setColor(Col);

}

/*
void lsreconstruction_precompute() {
    signs = data.array() / (data.array().pow(2) + 1e-5).sqrt();
}
*/





ImVec4 clear_color = ImColor(114, 144, 154);






void do_animation() {
}

void render(int width, int height) {
    if(animate) {
        do_animation();
    }
    // Rendering
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

    set_mvp(width,height);


    renderer->set_mvp(cam.mvp());
    renderer->set_mvp(cam.mv(),cam.p());
    bbox_renderer->set_mvp(cam.mvp());

    renderer->set_mvp(cam.mvp());
    renderer->set_mvp(cam.mv(),cam.p());
    bbox_renderer->set_mvp(cam.mvp());
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    //renderer->render();
    
    glDepthFunc(GL_LESS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_CLIP_PLANE0);

    //glClipPlane(GL_CLIP_PLANE0,glm::value_ptr(clip_plane_equation));
    renderer->render();
    //glDisable(GL_CLIP_PLANE0);
    //bbox_renderer->render();



}


void gui_func() {
    {
        ImGui::Text("Hello, world!");

        auto&& io = ImGui::GetIO();

        ImGui::Checkbox("animate", &animate);
        //mtao::opengl::imgui::SliderDouble4("Clip Plane",glm::value_ptr(clip_plane_equation),0,1);


        renderer->imgui_interface();


        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        //auto gpos = grid_mouse_pos();
        //glm::ivec2 gposi;
        //gposi.x = std::max<int>(0,std::min<int>(NI-1,gpos.x));
        //gposi.y = std::max<int>(0,std::min<int>(NJ-1,gpos.y));
        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS) [%.3f,%.3f]", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate,gpos.x,gpos.y);
        //ImGui::Text("[%d,%d]: %.5f", gposi.x,gposi.y,data(gposi.x,gposi.y));







    }
    if(ImGui::Button("Reset  Translation?")) {
        cam.reset();
    }
    if(ImGui::Button("Step")) {
        do_animation();
    }
    if(ImGui::Button("norm")) {
    }
    if(ImGui::Button("Reset")) {
    }
}
void set_keys() {
    auto&& h= window->hotkeys();

}


int main(int argc, char * argv[]) {

    set_opengl_version_hints(4,5);
    window = std::make_unique<Window>();
    set_keys();
    window->set_gui_func(gui_func);
    window->set_render_func(render);
    window->makeCurrent();

    prepare_mesh(NI,NJ,NK);
    window->run();

    exit(EXIT_SUCCESS);
}


