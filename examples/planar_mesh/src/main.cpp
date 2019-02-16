#include "mtao/opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include "mtao/opengl/shader.h"
#include "mtao/opengl/vao.h"
#include "mtao/opengl/bo.h"
#include "mtao/geometry/volume.h"
#include <memory>
#include <algorithm>

#include <mtao/cmdline_parser.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include "mtao/opengl/renderers/mesh.h"
#include <mtao/type_utils.h>
#include "mtao/opengl/camera.hpp"
#include <mtao/geometry/mesh/triangle/triangle_wrapper.h>
#include <set>
#include <iterator>
#include "example_boundaries.h"
#include <mtao/logging/timer.hpp>
//#include <igl/colormap.h>

using namespace mtao::geometry::mesh::triangle;

using namespace mtao::opengl;
using namespace mtao::logging;



glm::mat4 mvp_it;
float rotation_angle;
glm::vec3 edge_color;
static char filename[128] = "output.simobj";

static char opt_cstr[128] = "zPa.01qepcDQ";
static triangle_opts tri_opts;


static std::string_view filename_view(filename,mtao::types::container_size(filename));
static std::string_view opt_view(opt_cstr,mtao::types::container_size(opt_cstr));

std::unique_ptr<renderers::MeshRenderer> renderer;
std::unique_ptr<renderers::MeshRenderer> voronoi_renderer;

std::unique_ptr<Window> window;
int square_side = 10;
int square_aspect = 4;
int circle_side = 20;
float mean_area = -1;
float mean_edge_length = -1;
float min_edge_length = std::numeric_limits<REAL>::max();
static bool show_delauney_failures = false;


Camera2D cam;

Mesh original;
std::optional<Mesh> triangulated;
bool triangulated_active = false;

int active_edge_marker = -1;

void write(const std::string& filename, bool extra_metadata = false) {
    auto&& m = *triangulated;
    std::ofstream of(filename);

    if(extra_metadata) {
        of << "#vertices faces edges" << std::endl;
        of << "c " << m.V.cols() << " " << m.F.cols() << " " << m.E.cols() << std::endl;
    }

    for(int i = 0; i < m.V.cols(); ++i) {
        of << "v "<< m.V.col(i).transpose() << " 0" << std::endl;
    }

    for(int i = 0; i < m.F.cols(); ++i) {
        if(extra_metadata) {
            of << "f "<< m.F.col(i).transpose().array() << std::endl;
        } else {
            of << "f "<< m.F.col(i).transpose().array()+1 << std::endl;
        }
    }

    if(extra_metadata) {
        for(int i = 0; i < m.E.cols(); ++i) {
            of << "e "<< m.E.col(i).transpose() << std::endl;
        }
        for(int i = 0; i < m.EA.rows(); ++i) {
            if(m.EA(i) == 1 || m.EA(i) == 3) {
                of << "db " << i << " 0 1 0"<< std::endl;
            }
        }

        for(int i = 0; i < m.EA.rows(); ++i) {
            if(m.EA(i) == 2 || m.EA(i) == 4) {
                of << "db " << i << " 1 0 0"<< std::endl;
            }
        }
    }
    debug() << "Wrote to a file: " << filename;
}

const Mesh& active_mesh() {
    if(triangulated_active && triangulated) {
        return *triangulated;
    } else {
        return original;
    }
}
void set_mvp(int w, int h) {
    cam.set_shape(w,h);
    auto&& m = cam.m();
    m = glm::mat4();
    m = glm::rotate(m,(float) rotation_angle,glm::vec3(0,0,1));

    //cam.v() = glm::lookAt(glm::vec3(1,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0));

    cam.pan();
    cam.update();

    mvp_it = glm::transpose(glm::inverse(cam.mvp()));

}

void reset_state() {

    triangulated = std::nullopt;
    mean_area = -1;
    mean_edge_length = -1;
    min_edge_length = std::numeric_limits<REAL>::max();
}


void marker_warning() {
    warn() << "Bad marker choice: " << active_edge_marker;
    auto&& m = active_mesh();
    std::set<int> avail;
    avail.insert(-1);
    for(int i =0; i < m.EA.rows(); ++i) {
        avail.insert(m.EA(i));
    }
    auto w = warn();
    for(auto&& a: avail) { w << a << ", "; }
}



void prepare_mesh() {
    auto& m = active_mesh();
    if(m.F.cols() > 0) {
        renderer->setMesh(m.V.cast<float>(),m.F.cast<unsigned int>(),false);
    } else {
        renderer->setVertices(m.V.cast<float>(),false);
    }
    auto&& C = *m.C;
    voronoi_renderer->setVertices(C.cast<float>());

    { auto t = mtao::logging::timer("Checking delauney");
    auto del = m.verify_delauney();
    int size = del.rows() - del.count();
    if(size > 0) {
        mtao::ColVectors<REAL,2> C2(2,size);
        int c2 = 0;
        for(int i = 0; i < del.size(); ++i) {
            if(!del(i)) {
                C2.col(c2++) = C.col(i);
            }
        }
        voronoi_renderer->setVertices(C2.cast<float>());
    }
    }


    auto&& E = m.E;
    auto&& EA = m.EA;
    if(active_edge_marker == -1) {
        renderer->setEdges(m.E.cast<unsigned int>());
    } else {
        int size = (EA.array() == active_edge_marker).count();
        if(size == 0) {
            marker_warning();
        } else {

            mtao::ColVectors<int,2> E2(2,size);
            int idx = 0;
            for(int i = 0; i <  EA.rows(); ++i) {
                if(EA(i) == active_edge_marker) {
                    E2.col(idx++) = E.col(i);
                }
            }
            renderer->setEdges(E2.cast<unsigned int>());
        }
    }
}



ImVec4 clear_color = ImColor(114, 144, 154);

void gui_func() {
    float look_min=0.0001f, look_max=100.0f;

    ImGui::SliderFloat("angle", &rotation_angle,0,M_PI,"%.3f");
    auto&& io = ImGui::GetIO();


    renderer->imgui_interface();



    int newidx = active_edge_marker;
    ImGui::InputInt("Edge marker", &newidx);
    if(newidx != active_edge_marker) {
        active_edge_marker = newidx;
        prepare_mesh();
    }

    ImGui::InputInt("Square side",&square_side);
    ImGui::InputInt("Square aspect",&square_aspect);
    ImGui::InputInt("Circle side",&circle_side);
    std::string oldopts = std::string(opt_view);
    ImGui::InputText("Triangle options",opt_cstr,128);
    if(oldopts != opt_view) {
        tri_opts.parse_options(opt_view);
    }

    {
        static bool show = true;
        ImGui::Begin("Triangle opts",&show); 
        tri_opts.imgui_interface();
        std::string newopts = tri_opts;
        std::copy(newopts.begin(),newopts.end(),opt_cstr);
        opt_cstr[newopts.size()] = '\0';
        ImGui::End();
    }

    if(ImGui::Button("Toggle triangulated")) {
        triangulated_active ^= true;
        if(!triangulated) {
            triangulated_active = false;
        }
        prepare_mesh();
    }

    ImGui::InputText("Filename",filename,128);
    if(ImGui::Button("Save")) {
        write(filename);
    }
    ImGui::Checkbox("Show Delauney Failures", &show_delauney_failures);
        if(show_delauney_failures) {
        voronoi_renderer->set_vertex_style(renderers::MeshRenderer::VertexStyle::Flat);
        voronoi_renderer->imgui_interface();
        auto& color = voronoi_renderer->vertex_color();

        ImGui::ColorEdit3("ambient", glm::value_ptr(color));

    } else {
        voronoi_renderer->set_vertex_style(renderers::MeshRenderer::VertexStyle::Disabled);
    }


    ImGui::ColorEdit3("clear color", (float*)&clear_color);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    glm::vec2 mouse_pos;
    mtao::Vector3<float> p;
    p.setZero();
    mouse_pos = cam.mouse_pos(ImGui::GetIO().MousePos);
    p.x() = mouse_pos.x;
    p.y() = mouse_pos.y;

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
    glDisable(GL_DEPTH_TEST);
    voronoi_renderer->set_mvp(cam.mvp());
    voronoi_renderer->set_mvp(cam.mv(),cam.p());
    voronoi_renderer->render();
    glEnable(GL_DEPTH_TEST);
    auto&& io = ImGui::GetIO();
    auto p = cam.mouse_pos(io.MousePos);
    mtao::logging::trace() << "Mouse coordinate: " << p.x << "," << p.y;

}


void triangulate() {

    debug() << "Triangulating!";
    triangulated = triangle_wrapper(original, tri_opts);
    debug() << bool(triangulated);
    auto&& m = *triangulated;
    auto&& F = m.F;
    auto&& E = m.E;
    auto&& V = m.V;
    mean_area = 0;
    mean_edge_length= 0;
    for(int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        mtao::Matrix<REAL,2,3> T;
        for(int j = 0; j < 3; ++j) {
            T.col(j) = V.col(f(j));
        }
        mean_area += std::abs(mtao::geometry::volume(T));
    }
    mean_area /= F.cols();
    for(int i = 0; i < E.cols(); ++i) {
        auto e = E.col(i);
        auto u = V.col(e(0));
        auto v = V.col(e(1));
        REAL el = (u-v).norm();
        mean_edge_length  += el;
        min_edge_length = std::min<float>(min_edge_length,el);
    }
    mean_edge_length /= E.cols();
}

void set_original(int idx) {

    reset_state();
    switch(idx) {
        case 0: original = make_box_circle(square_aspect,square_side,circle_side);break;
        case 1: original = make_circle_circle(circle_side); break;
        case 2: original = make_box(square_side,square_aspect); break;
        case 3: original = make_circle(circle_side); break;
    }
    prepare_mesh();
}

void set_keys() {
    auto&& h= window->hotkeys();

    h.add([&](){set_original(0);},"Use box circle", GLFW_KEY_1);
    h.add([&](){set_original(1);},"Use box circle", GLFW_KEY_2);
    h.add([&](){set_original(2);},"Use box", GLFW_KEY_3);
    h.add([&](){set_original(3);},"Use circle", GLFW_KEY_4);
    h.add([&]() {
            triangulate();
            triangulated_active = true;
            prepare_mesh();
            },"Triangulate", GLFW_KEY_T);
    h.add([&]() {
            triangulated_active ^= true;
            if(!triangulated) {
                triangulated_active = false;
            }
            prepare_mesh();
            },"Toggle Triangulation", GLFW_KEY_SPACE);

}

void print_hotkeys() {
    std::cout << window->hotkeys().info() << std::endl;
}

int main(int argc, char * argv[]) {
    tri_opts.parse_options(opt_view);

    mtao::CommandLineParser clp;
    clp.add_option("writeall");
    clp.add_option("tri_opts",std::string(opt_view));
    clp.parse(argc,argv);

    if(clp.optT<bool>("writeall")) {
        debug() << "Writing all files!";
        {
            original = make_box_circle(square_aspect,square_side,circle_side); 
            triangulate();
            write("box_circle.obj");
        }
        {
            original = make_circle_circle(circle_side);
            triangulate();
            write("circle_circle.obj");
        }
        {
            original = make_box(square_side,square_aspect);
            triangulate();
            write("box.obj");
        }
        {
            original = make_circle(circle_side);
            triangulate();
            write("circle.obj");
        }

        return 0;
    } else {
        active_loggers["default"].set_level(Level::Debug);
        debug() << "Should show window!";

        window = std::make_unique<Window>();
        window->set_gui_func(gui_func);
        window->set_render_func(render);
        window->makeCurrent();
        set_keys();



        renderer = std::make_unique<renderers::MeshRenderer>(2);
        voronoi_renderer = std::make_unique<renderers::MeshRenderer>(2);
        renderer->unset_all();
        voronoi_renderer->unset_all();
        renderer->set_edge_style(renderers::MeshRenderer::EdgeStyle::Mesh);
        //renderer->set_vertex_style(renderers::MeshRenderer::VertexStyle::Flat);
        voronoi_renderer->set_vertex_style(renderers::MeshRenderer::VertexStyle::Flat);
        original = make_box_circle(square_aspect,square_side,circle_side);
        voronoi_renderer->vertex_color() = glm::vec4(1,0,0,1);

        window->run();

        exit(EXIT_SUCCESS);
    }
}


