#include "opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include "opengl/shader.h"
#include "opengl/vao.h"
#include "opengl/bo.h"
#include <memory>
#include "mesh.h"
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

using namespace mtao::opengl;



float edge_threshold = 0.001;
float mean_edge_length = 1.0;
float look_distance = 0.4;
glm::mat4 m,v,mv,p,mvp, mvp_it;
float rotation_angle;
glm::vec3 edge_color;


std::unique_ptr<VAO> vertex_attribute;
std::unique_ptr<IBO> index_buffer;
std::unique_ptr<IBO> edge_index_buffer;
std::unique_ptr<BO> vertex_buffer;
std::unique_ptr<ShaderProgram> program;

std::unique_ptr<ShaderProgram> edge_program;
std::unique_ptr<Window> window;


bool index_buffer_active = false;
bool use_barycentric_edges = true;

void set_mvp(int w, int h) {
    m = glm::mat4();
    m = glm::rotate(m,(float) rotation_angle,glm::vec3(0,0,1));
    //m = glm::rotate(m,(float) glfwGetTime() / 5.0f,glm::vec3(0,1,0));
    float ratio = w / (float) h;
    //p = glm::perspective(45.f,ratio,.1f,10.0f);
    p = glm::ortho(-ratio*look_distance,ratio*look_distance,-look_distance,look_distance,1.f,-1.f);

    /*
    v = glm::lookAt(glm::vec3(0,0,look_distance), glm::vec3(0,0,0), glm::vec3(0,1,0));
    */
    mv = v*m;
    mvp = p * mv;
    mvp_it = glm::transpose(glm::inverse(mvp));
}

auto prepareShaders(const char* vdata, const char* fdata, const char* geo = nullptr) {

    Shader vertex_shader(GL_VERTEX_SHADER);
    vertex_shader.compile(&vdata);
    Shader fragment_shader (GL_FRAGMENT_SHADER);
    fragment_shader.compile(&fdata);

    std::unique_ptr<Shader> geometry_shader;
    if(geo) {
        geometry_shader = std::make_unique<Shader>(GL_GEOMETRY_SHADER);
        geometry_shader->compile(&geo);
    }


    auto program = std::make_unique<ShaderProgram>();
    program->attach(vertex_shader);
    program->attach(fragment_shader);
    if(geometry_shader) {
        program->attach(*geometry_shader);
    }
    program->compile();


    vertex_attribute = std::make_unique<VAO>();
    vertex_attribute->bind();


    return program;


}

auto make_bary_edge_shader() {
    std::cout << "Bary Edge shaders: " << std::endl;
    static const char* vertex_shader_text =
        "#version 330\n"
        "uniform mat4 MVP;\n"
        "in vec2 vPos;\n"
        "out vec4 pos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = MVP * vec4(vPos,0.0, 1.0);\n"
        "    pos = MVP * vec4(vPos,0.0, 1.0);\n"
        "}\n";

    static const char* geometry_shader_text =
        "#version 330\n"
        "layout(triangles) in;\n"
        "layout(triangle_strip,max_vertices = 3) out;\n"
        "in vec4 pos[3];\n"
        "out vec3 bary;\n"
        "uniform float mean_edge_length;\n"
        "void main()\n"
        "{\n"
        "   int i;"
        "   vec3 lens;"
        "   lens.x = length(pos[2]-pos[1]);\n"
        "   lens.y = length(pos[0]-pos[2]);\n"
        "   lens.z = length(pos[0]-pos[1]);\n"
        "   float s = (lens.x + lens.y + lens.z)/2;\n"
        "   float area = sqrt(s * (s - lens.x) * (s - lens.y) * (s - lens.z));\n"
        "   lens = (2 * area) / lens;\n"
        "   for(i = 0; i < 3; i++)\n"
        "   {\n"
        "       bary = vec3(0);\n"
        "       bary[i] = lens[i] / mean_edge_length;\n"
        //"       bary[i] = 1.0;\n"
        "       gl_Position = pos[i];\n"
        "       EmitVertex();\n"
        "   }\n"
        "EndPrimitive();\n"
        "}\n";

    static const char* fragment_shader_text =
        "#version 330\n"
        "uniform vec3 color;\n"
        "uniform float thresh;\n"
        "in vec3 bary;\n"
        "out vec4 out_color;\n"
        "void main()\n"
        "{\n"
        "   if(min(bary.x,min(bary.y,bary.z)) < thresh){\n"
        "       out_color= vec4(color,1.0);\n"
        "   } else {\n"
        "       discard;\n"
        "   }\n"
        "}\n";

    return prepareShaders(vertex_shader_text,fragment_shader_text, geometry_shader_text);
}

auto make_edge_shader() {
    std::cout << "Edge shaders: " << std::endl;
        static const char* vertex_shader_text =
            "#version 330\n"
            "uniform mat4 MVP;\n"
            "in vec2 vPos;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
            "}\n";

        static const char* fragment_shader_text =
            "#version 330\n"
            "uniform vec3 color;\n"
            "out vec4 out_color;\n"
            "void main()\n"
            "{\n"
            "    out_color= vec4(color,1.0);\n"
            "}\n";

        return prepareShaders(vertex_shader_text,fragment_shader_text);
}

void prepare_edge_shader(const Mesh& m) {

    if(use_barycentric_edges) {
    edge_program = make_bary_edge_shader();;

        mean_edge_length = 0;//technically we want the mean dual edge length
        for(int i = 0; i < m.F.cols(); ++i) {
            for(int j = 0; j < 3; ++j) {
                int a = m.F(j,i);
                int b = (m.F(j,i)+1)%3;
                mean_edge_length += (m.V.col(a) - m.V.col(b)).norm();
            }
        }
        mean_edge_length /= m.F.size();

        {auto p = edge_program->useRAII();
            edge_program->getAttrib("vPos").setPointer(3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);
            edge_program->getUniform("mean_edge_length").set(mean_edge_length);
        }

    } else {

        edge_program = make_edge_shader();

        //compute edges
        Eigen::Matrix<GLuint,Eigen::Dynamic,Eigen::Dynamic> E;
        E.resize(2,3*m.F.cols());

        E.leftCols(m.F.cols()) = m.F.topRows(2);
        E.block(0,m.F.cols(),1,m.F.cols()) = m.F.row(0);
        E.block(1,m.F.cols(),1,m.F.cols()) = m.F.row(2);
        E.rightCols(m.F.cols()) = m.F.bottomRows(2);

        edge_index_buffer = std::make_unique<IBO>(GL_LINES);
        edge_index_buffer->bind();
        edge_index_buffer->setData(E.data(),sizeof(GLuint) * E.size());
    }


}




void prepare_mesh(const Mesh& m) {
    prepare_edge_shader(m);
    std::cout << "Mesh shaders: " << std::endl;
    static const char* vertex_shader_text =
        "#version 330\n"
        "uniform mat4 MVP;\n"
        "uniform vec2 minPos;\n"
        "uniform vec2 range;\n"
        "in vec2 vPos;\n"
        "out vec2 color;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = MVP * vec4(vPos,0.0, 1.0);\n"
        "    color = (vec2(vPos) - minPos)/range;\n"
        "}\n";
    static const char* fragment_shader_text =
        "#version 330\n"
        "in vec2 color;\n"
        "out vec4 out_color;\n"
        "void main()\n"
        "{\n"
        "    out_color= vec4(color,0.0,1.0);\n"
        "}\n";

    program = prepareShaders(vertex_shader_text,fragment_shader_text);


    vertex_buffer = std::make_unique<BO>();
    vertex_buffer->bind();
    vertex_buffer->setData(m.V.data(),sizeof(float) * m.V.size());

    index_buffer = std::make_unique<IBO>(GL_TRIANGLES);
    index_buffer->bind();
    index_buffer->setData(m.F.data(),sizeof(int) * m.F.size());
    index_buffer_active = true;



    Eigen::Matrix<GLfloat,2,1> minPos,maxPos,range;
    minPos = m.V.rowwise().minCoeff();
    maxPos = m.V.rowwise().maxCoeff();
    range = maxPos - minPos;



    {auto p = program->useRAII();

        program->getAttrib("vPos").setPointer(2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) 0);


        program->getUniform("minPos").set2f(minPos.x(),minPos.y());
        program->getUniform("range").set2f(range.x(),range.y());
    }
}



ImVec4 clear_color = ImColor(114, 144, 154);

void gui_func() {
    {
        float look_min=0.0001f, look_max=100.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("edge_threshold", &edge_threshold, 0.0f, 0.01f,"%.5f");

        ImGui::SliderFloat("look_distance", &look_distance, look_min,look_max,"%.3f");
        ImGui::SliderFloat("angle", &rotation_angle,0,M_PI,"%.3f");
        auto&& io = ImGui::GetIO();
        look_distance += .5 * io.MouseWheel;
        look_distance = std::min(std::max(look_distance,look_min),look_max);




        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::ColorEdit3("edge color", glm::value_ptr(edge_color));
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

    set_mvp(width,height);


    { auto program_active = edge_program->useRAII();

        auto vpos_active = edge_program->getAttrib("vPos").enableRAII();

        edge_program->setUniform("mean_edge_length",mean_edge_length);

        edge_program->getUniform("color").setVector(edge_color);
        edge_program->getUniform("thresh").set(edge_threshold);
        edge_program->getUniform("MVP").setMatrix(mvp);

        if(index_buffer_active) {
            index_buffer->drawElements();
        }

    }

    { auto program_active = program->useRAII();

        auto vpos_active = program->getAttrib("vPos").enableRAII();
        program->getUniform("MVP").setMatrix(mvp);

        if(index_buffer_active) {
            index_buffer->drawElements();
        } else {
            auto vpos_active = program->getAttrib("vCol").enableRAII();
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

    }


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


