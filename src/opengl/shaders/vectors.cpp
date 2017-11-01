#include "opengl/shaders.h"
#include <sstream>



namespace mtao {namespace opengl {namespace shaders {
    Shader vector_vertex_shader(int dim) {
        std::stringstream ss;

        ss <<  "#version 330\n"
        if(dim == 2) {
            ss << "in vec2 vPos;\n";
            ss << "in vec2 vVec;\n";
        } else {
            ss << "in vec3 vPos;\n";
            ss << "in vec3 vVec;\n";
        }
        ss << "in vec3 vColor;\n";
        ss << "out vec3 gPos;\n";
        ss << "out vec3 gVec;\n";
        ss <<  "void main()\n"
            "{\n";
        if(dim == 2) {
            ss << "    gPos = vec3(vPos, 0.0);\n";
            ss << "    gVec = vec3(vVec, 0.0);\n";
        } else {
            ss << "    gPos = vPos;
            ss << "    gVec = vVec;
        }
        ss << "}\n";
        return prepareShader(ss.str().c_str(), GL_VERTEX_SHADER);
    }

    //shader is prepared for a custsom fragment shader but won't write that now
    Shader vector_geometry_shader() {
        static const char* text =
            "#version 330\n"
            "layout(points) in;\n"
            "layout(line_strip,max_vertices = 3) out;\n"
            "in vec3 gPos[1];\n"
            "in vec3 gVec[1];\n"
            "uniform mat4 MVP;\n"
            "out float fMagnitude;\n"
            "out float fBary;\n"
            "uniform vec3 color;\n"
            "out vec3 fColor;\n"
            "void main()\n"
            "{\n"
            "   fMagnitude = length(gVec);\n"
            "   fBary = 0;\n"
            "   fColor = fMagnitude * (1-fBary) * color;\n"
            "   gl_Position = MVP * vec4(gPos[0],1);\n"
            "   EmitVertex();\n"
            "   fBary = 1;\n"
            "   fColor = fMagnitude * (1-fBary) * color;\n"
            "   gl_Position = MVP * vec4(gPos[0] + gVec[0],1);\n"
            "   EmitVertex();\n"
            "   EndPrimitive();\n"
            "}\n";
        return prepareShader(text, GL_GEOMETRY_SHADER);
    }

    ShaderProgram vector_shader_program(bool per_vertex, int dim) {
        auto v = vector_vertex_shader(dim);
        auto g = vector_geometry_shader(dim);
        auto f = color_fragment_shader(per_vertex);
        return linkShaderProgram(v,g,f);
    }
}}}
