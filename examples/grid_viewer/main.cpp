#include "mtao/opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include "mtao/opengl/shader.h"
#include "mtao/opengl/vao.h"
#include "mtao/opengl/bo.h"
#include <mtao/eigen/stack.h>
#include <memory>
#include "grid.h"
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include "mtao/opengl/renderers/mesh.h"
#include "mtao/opengl/camera.hpp"
#include <mtao/eigen_utils.h>

using namespace mtao::opengl;



float look_distance = 0.6;
glm::mat4 mvp_it;
float rotation_angle;
glm::vec3 edge_color;
bool animate = false;
using Mat = mtao::MatrixX<GLfloat>;
Mat data;
Mat data_original;
Mat dx;
Mat dy;
Mat signs;

int NI=200;
int NJ=200;

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





void set_colors(const Mat& data) {
    mtao::RowVectorX<GLfloat> col = Eigen::Map<const mtao::RowVectorX<GLfloat>>(data.data(),data.size());

    mtao::MatrixX<GLfloat> Col = mtao::eigen::vstack((col.array()>0).select(col,0),-(col.array()<0).select(col,0),mtao::RowVectorX<GLfloat>::Zero(data.size()));

    Col.array() = Col.array().pow(.5);
    renderer->setColor(Col);
}

void prepare_mesh(int i, int j) {

    renderer = std::make_unique<renderers::MeshRenderer>(2);

    auto [V,F] = make_mesh(i,j);
    V.array() -= .5;

    mtao::RowVectorX<GLfloat> col = V.colwise().norm().array() - GLfloat(.25);
    data = Eigen::Map<Mat>(col.data(),i,j);

    signs = data.array() / (data.array().pow(2) + 1e-5).sqrt();



    data = data.array().pow(3);

    renderer->setMesh(V,F,false);
    renderer->set_face_style(renderers::MeshRenderer::FaceStyle::Color);
    renderer->set_vertex_style();
    set_colors(data);
    data_original = data;
}






ImVec4 clear_color = ImColor(114, 144, 154);



void centered_difference() {

    auto dx_ = (NI-1)*(data.rightCols(NJ-1) - data.leftCols(NJ-1));
    auto dy_ = (NJ-1)*(data.topRows(NI-1) - data.bottomRows(NI-1));
    dx.resizeLike(data);
    dy.resizeLike(data);
    dx.setZero();
    dy.setZero();
    dx.leftCols(NJ-1) = dx_;
    dx.rightCols(NJ-1) += dx_;
    dy.topRows(NI-1) = dy_;
    dy.bottomRows(NI-1) += dy_;
    dx.block(0,1,NI,NJ-2) /= 2;
    dy.block(1,0,NI-2,NJ) /= 2;
}


Mat G() {
    centered_difference();
    //return (dx.array().pow(2) + dy.array().pow(2)).sqrt() - 1;
    auto dx_ = (NI-1)*(data.rightCols(NJ-1) - data.leftCols(NJ-1));
    auto dy_ = (NJ-1)*(data.topRows(NI-1) - data.bottomRows(NI-1));

    Mat a(data.rows(),data.cols());
    Mat b(data.rows(),data.cols());
    Mat c(data.rows(),data.cols());
    Mat d(data.rows(),data.cols());
    a.setZero();
    b.setZero();
    c.setZero();
    d.setZero();

    dx.resizeLike(data);
    dy.resizeLike(data);
    dx.setZero();
    dy.setZero();
    a.leftCols(NJ-1) = dx_;
    b.rightCols(NJ-1) = dx_;
    c.bottomRows(NI-1) = dy_;
    d.topRows(NI-1) = dy_;

    auto ap = a.array().max(0).pow(2);
    auto am = a.array().min(0).pow(2);
    auto bp = b.array().max(0).pow(2);
    auto bm = b.array().min(0).pow(2);
    auto cp = c.array().max(0).pow(2);
    auto cm = c.array().min(0).pow(2);
    auto dp = d.array().max(0).pow(2);
    auto dm = d.array().min(0).pow(2);

    return mtao::eigen::finite((data_original.array() <= 0).select
    (ap.max(bm) + cp.max(dm),
    bp.max(am) + dp.max(cm)) - 1);


    //dy.block(1,0,NI-2,NJ) /= 2;
}


void do_animation() {

    centered_difference();


    Mat D = signs.cwiseProduct(G());
    std::cout << "Mean error: " << D.mean() << std::endl;

    data -= 1.0 / (4 * std::max(NI,NJ)) * D;
    set_colors(data);
}

void render(int width, int height) {
    if(animate) {
        do_animation();
    }
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


void gui_func() {
    {
        float look_min=0.0001f, look_max=100.0f;
        ImGui::Text("Hello, world!");

        ImGui::SliderFloat("look_distance", &look_distance, look_min,look_max,"%.3f");
        ImGui::SliderFloat("angle", &rotation_angle,0,M_PI,"%.3f");
        auto&& io = ImGui::GetIO();
        look_distance += .5 * io.MouseWheel;
        look_distance = std::min(std::max(look_distance,look_min),look_max);

        ImGui::Checkbox("animate", &animate);

        renderer->imgui_interface();


        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    }
    if(ImGui::Button("Reset  Translation?")) {
        cam.reset();
    }
    if(ImGui::Button("Step")) {
        do_animation();
    }
    if(ImGui::Button("norm")) {
        Mat n= (dx.array().pow(2) + dy.array().pow(2));
        set_colors(n);
    }
    if(ImGui::Button("dx")) {
        set_colors(dx);
    }
    if(ImGui::Button("dy")) {
        set_colors(dy);
    }
    if(ImGui::Button("Reset")) {
        data = data_original;
    }
}

int main(int argc, char * argv[]) {

    window = std::make_unique<Window>();
    window->set_gui_func(gui_func);
    window->set_render_func(render);
    window->makeCurrent();

    prepare_mesh(NI,NJ);
    window->run();

    exit(EXIT_SUCCESS);
}


