#ifndef SHADERS_H
#define SHADERS_H
#include <string>
#include "opengl/shader.h"

namespace mtao {namespace opengl {namespace shaders {

    Shader simple_vertex_shader(int dim);

    Shader single_color_fragment_shader();
    Shader attribute_color_fragment_shader();
    Shader color_fragment_shader(bool per_vertex);
    ShaderProgram color_fragment_shader_program(bool per_vertex, int dim);

    Shader barycentric_edge_geometry_shader();
    Shader barycentric_edge_fragment_shader();
    ShaderProgram barycentric_edge_shader_program(int dim);

    Shader phong_fragment_shader();
    ShaderProgram phong_shader_program(int dim);



}}}
#endif//SHADERS_H
