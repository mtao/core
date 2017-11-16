#include "opengl/shaders.h"
#include <sstream>



namespace mtao {namespace opengl {namespace shaders {
    Shader barycentric_edge_geometry_shader() {
        static const char* text =
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
        return prepareShader(text, GL_GEOMETRY_SHADER);
    }

    Shader barycentric_edge_fragment_shader() {
        static const char* text =
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
        return prepareShader(text, GL_FRAGMENT_SHADER);
    }
    ShaderProgram barycentric_edge_shader_program(int dim) {
        auto v = simple_vertex_shader(dim);
        auto g = barycentric_edge_geometry_shader();
        auto f = barycentric_edge_fragment_shader();
        return linkShaderProgram(v,g,f);
    }

}}}

