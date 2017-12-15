#include "mtao/opengl/shaders.h"
#include <sstream>



namespace mtao {namespace opengl {namespace shaders {
    Shader simple_vertex_shader(int dim) {
        std::stringstream ss;

        ss <<  "#version 330\n"
            "uniform mat4 MVP;\n";
        if(dim == 2) {
            ss << "in vec2 vPos;\n";
        } else {
            ss << "in vec3 vPos;\n";
            ss << "in vec3 vNormal;\n";
        }
        ss << "in vec3 vColor;\n";
        ss << "out vec3 fNormal;\n";
        ss << "out vec3 fPos;\n";
        ss << "out vec4 gPos;\n";
        ss << "out vec3 fColor;\n";
        ss <<  "void main()\n"
            "{\n";
        if(dim == 2) {
            ss << "    gPos = MVP * vec4(vPos, 0.0, 1.0);\n";
        } else {
            ss << "    gPos = MVP * vec4(vPos, 1.0);\n";
        }
        ss << "    gl_Position = gPos;\n";
        if(dim == 2) {
            ss << "    fPos = vec3(vPos,0);\n";
            ss << "    fNormal = vec3(0,0,1);\n";
        } else {
            ss << "    fPos = vPos;\n";
            ss << "    fNormal = vNormal;\n";
            ss << "    fColor = vColor;\n";
        }
        ss << "}\n";
        return prepareShader(ss.str().c_str(), GL_VERTEX_SHADER);
    }

    Shader single_color_fragment_shader() {
        return color_fragment_shader(false);
    }
    Shader attribute_color_fragment_shader() {
        return color_fragment_shader(true);
    }
    Shader color_fragment_shader(bool per_vertex) {
        std::stringstream ss;

        ss <<  "#version 330\n";
        ss << "out vec4 out_color;\n";
        if(per_vertex) {
            ss << "in vec3 fColor;\n";
        } else {
            ss << "uniform vec3 color;\n";
        }
        ss <<  "void main()\n";
            ss << "{\n";
        if(per_vertex) {
            ss << "    out_color= vec4(fColor,1.0);\n";
        } else {
            ss << "    out_color= vec4(color,1.0);\n";
        }
        ss << "}\n";
        return prepareShader(ss.str().c_str(), GL_FRAGMENT_SHADER);
    }

    ShaderProgram color_fragment_shader_program(bool per_vertex, int dim) {
        auto v = simple_vertex_shader(dim);
        auto f = color_fragment_shader(per_vertex);
        return linkShaderProgram(v,f);
    }
}}}
