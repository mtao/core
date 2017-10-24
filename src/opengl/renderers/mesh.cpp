#include "opengl/renderers/mesh.h"
#include <exception>
#include <stdexcept>
#include <sstream>
#include <list>

namespace mtao { namespace opengl { namespace renderers {


MeshRenderer::MeshRenderer(int dim) {
    if(dim == 2 || dim == 3) {
        loadShaders(dim);
    } else {
        throw std::invalid_argument("Meshes have to be dim 2 or 3");
    }
}


void MeshRenderer::loadShaders(int dim) {
    std::stringstream vss;

    vss <<  "#version 330\n"
        "uniform mat4 MVP;\n";
    if(dim == 2) {
        vss << "in vec2 vPos;\n";
    } else {
        vss << "in vec3 vPos;\n";
    }
    vss << "out vec4 gPos;\n";
    vss <<  "void main()\n"
        "{\n";
    if(dim == 2) {
        vss << "    gPos = MVP * vec4(vPos, 0.0, 1.0);\n";
    } else {
        vss << "    gPos = MVP * vec4(vPos, 1.0);\n";
    }
        vss << "    gl_Position = gPos;\n";
    vss << "}\n";


    static const char* flat_fragment_shader_text =
        "#version 330\n"
        "uniform vec3 color;\n"
        "out vec4 out_color;\n"
        "void main()\n"
        "{\n"
        "    out_color= vec4(color,1.0);\n"
        "}\n";

    static const char* baryedge_geometry_shader_text =
        "#version 330\n"
        "layout(triangles) in;\n"
        "layout(triangle_strip,max_vertices = 3) out;\n"
        "in vec4 gPos[3];\n"
        "out vec3 bary;\n"
        "uniform float mean_edge_length;\n"
        "void main()\n"
        "{\n"
        "   int i;"
        "   vec3 lens;"
        "   lens.x = length(gPos[2]-gPos[1]);\n"
        "   lens.y = length(gPos[0]-gPos[2]);\n"
        "   lens.z = length(gPos[0]-gPos[1]);\n"
        "   float s = (lens.x + lens.y + lens.z)/2;\n"
        "   float area = sqrt(s * (s - lens.x) * (s - lens.y) * (s - lens.z));\n"
        "   lens = (2 * area) / lens;\n"
        "   for(i = 0; i < 3; i++)\n"
        "   {\n"
        "       bary = vec3(0);\n"
        "       bary[i] = lens[i] / mean_edge_length;\n"
        //"       bary[i] = 1.0;\n"
        "       gl_Position = gPos[i];\n"
        "       EmitVertex();\n"
        "   }\n"
        "EndPrimitive();\n"
        "}\n";

    static const char* baryedge_fragment_shader_text =
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

    auto vertex_shader = prepareShader(vss.str().c_str(), GL_VERTEX_SHADER);
    auto flat_fragment_shader = prepareShader(flat_fragment_shader_text, GL_FRAGMENT_SHADER);
    auto baryedge_fragment_shader = prepareShader(baryedge_fragment_shader_text, GL_FRAGMENT_SHADER);
    auto baryedge_geometry_shader = prepareShader(baryedge_geometry_shader_text, GL_GEOMETRY_SHADER);

    m_flat_renderer = linkShaderProgram(vertex_shader,flat_fragment_shader);
    m_baryedge_renderer = linkShaderProgram(vertex_shader, baryedge_fragment_shader, baryedge_geometry_shader);

}


        void MeshRenderer::render() const {
        }
        void MeshRenderer::set_mvp(const glm::mat4& mvp) {
            std::list<ShaderProgram*> shaders({m_flat_renderer.get(), m_baryedge_renderer.get()});
            for(auto&& p: shaders) {
                auto active = p->useRAII();
                p->getUniform("MVP").setMatrix(mvp);
            }
        }
        void MeshRenderer::set_face_color(const glm::vec4& col) {
            m_face_color = col;
        }
        void MeshRenderer::set_edge_color(const glm::vec4& col) {
            m_edge_color = col;
        }
}}}
