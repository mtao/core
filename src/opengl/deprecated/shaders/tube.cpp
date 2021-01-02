#include "mtao/opengl/shaders.h"
#include <sstream>

namespace mtao {
namespace opengl {
    namespace shaders {
        Shader tube_vertex_shader() {
            static const char *text =
              "#version 330\n"
              "in vec3 vPos;\n"
              "in vec3 vTan;\n"
              "in vec3 vNormal;\n"
              "out vec3 pos;\n"
              "out vec3 tan;\n"
              "out vec3 normal;\n"
              "void main()\n"
              "{\n"
              "    pos = vPos;\n"
              "    tan = vTan;\n"
              "    normal = vNormal;\n"
              "}\n";
            return prepareShader(text, GL_VERTEX_SHADER);
        }

        Shader tube_geometry_shader() {
            static const char *text =
              "#version 330\n"
              "layout(lines) in;\n"
              //"layout(points,max_vertices = 48) out;\n"
              "layout(triangle_strip,max_vertices = 256) out;\n"
              "in vec3 pos[2];\n"
              "in vec3 tan[2];\n"
              "in vec3 normal[2];\n"
              "out vec3 frag_normal;\n"
              "out vec3 frag_pos;\n"
              "uniform mat4 MVP;\n"
              "uniform float radius;\n"
              "int size=16;\n"
              "vec3 N[32];\n"//2 * size
              "vec3 Normal(vec3 on, vec3 bn, float t) {\n"
              "   return (cos(t) * on + sin(t) * bn);\n"
              "}\n"
              "void get_vertex(int i, int j) {\n"
              "   vec3 off = N[(size*i) + (j%size)];\n"
              "   frag_pos = pos[i] + radius * off;\n"
              "   gl_Position = MVP * vec4(frag_pos,1);EmitVertex();\n"
              "}\n"
              "void side_vertex(int i, int j) {\n"
              "   frag_normal = N[(size*i) + (j%size)];\n"
              "   get_vertex(i,j);\n"
              "}\n"
              "void main()\n"
              "{\n"
              "   for(int i = 0; i < 2; ++i) {\n"
#ifdef USE_STITCHED_ENDS
              "       vec3 t = tan[i];\n"
#else//USE_STITCHED_CAPS
              "       vec3 t = pos[1] - pos[0];\n"
#endif//USE_STITCHED_CAPS
              "       vec3 on = normal[i];\n"
              "       on = normalize(on);\n"
              "       vec3 bn = normalize(cross(t,on));\n"
              "       for(int j = 0; j < size; ++j) {\n"
              "           float time = 2 * j * 3.1415926535 / size;\n"
              "           N[i*size+j] = Normal(on,bn,time);\n"
              "       }\n"
              "   }\n"
              "   vec3 a = pos[0];\n"
              "   vec3 b = pos[1];\n"
              "   for(int j = 0; j < size; ++j) {\n"
#ifdef USE_RENDERED_CAPS
              "       frag_normal = tan[0]; frag_pos = pos[0];\n"
              "       gl_Position = MVP * vec4(pos[0],1);  EmitVertex();\n"
              "       get_vertex(0,j);\n"
              "       get_vertex(0,j+1);\n"
              "       EndPrimitive();\n"
              "       frag_normal = tan[1]; frag_pos = pos[1];\n"
              "       gl_Position = MVP * vec4(pos[1],1);EmitVertex();\n"
              "       get_vertex(1,j);\n"
              "       get_vertex(1,j+1);\n"
              "       EndPrimitive();\n"
#endif
              "       side_vertex(0,j);\n"
              "       side_vertex(1,j);\n"
              "       side_vertex(1,j+1);\n"
              "       EndPrimitive();\n"
              "       side_vertex(0,j);\n"
              "       side_vertex(1,j+1);\n"
              "       side_vertex(0,j+1);\n"
              "       EndPrimitive();\n"
              "   }\n"
              //"   EndPrimitive();\n"
              "}\n";
            return prepareShader(text, GL_GEOMETRY_SHADER);
        }
        ShaderProgram tube_shader_program() {
            auto v = tube_vertex_shader();
            auto g = tube_geometry_shader();
            auto f = phong_fragment_shader();
            return linkShaderProgram(v, g, f);
        }
    }// namespace shaders
}// namespace opengl
}// namespace mtao
