#ifndef SHADERS_H
#define SHADERS_H
#include <string>
#include "opengl/shader.h"

namespace mtao {namespace opengl {namespace shaders {

    Shader simple_vertex_shader(int dim);

    Shader single_color_fragment_shader();
    Shader attribute_color_fragment_shader();
    Shader color_fragment_shader(bool per_vertex);

    Shader barycentric_edge_geometry_shader();
    Shader barycentric_edge_fragment_shader();

    Shader phong_fragment_shader();


}}}
#endif//SHADERS_H
