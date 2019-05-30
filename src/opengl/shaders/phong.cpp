#include "mtao/opengl/shaders.h"
#include <sstream>



namespace mtao {namespace opengl {namespace shaders {

    Shader phong_fragment_shader() {

    static const char* text =
        "#version 330\n"
        //            "uniform vec3 color;\n"
        "uniform mat4 MV;\n"
        "uniform vec4 specularMaterial;\n"
        "uniform float specularExpMaterial;\n"
        "uniform vec4 diffuseMaterial;\n"
        "uniform vec4 ambientMaterial;\n"
        "uniform vec3 lightPos;\n"
        "in vec3 fNormal;\n"
        "in vec3 fPos;\n"
        "out vec4 out_color;\n"
        "void main()\n"
        "{\n"
        "   vec3 ambient = ambientMaterial.xyz;\n"
        "   vec3 eyeDir = normalize(fPos);\n"
        "   vec4 ldir = MV * vec4(lightPos - fPos,1);\n"
        "   vec3 lightDir = normalize(ldir.xyz/ldir.w);\n"

        "   float lightAng = max(0.0,dot(lightDir,fNormal));\n"
        "   if(lightAng == 0) {\n"
        "       out_color = vec4(ambient,1);\n"
        "       return;\n"
        "   }\n"

        "   vec3 diffuse = lightAng * diffuseMaterial.xyz;\n"

        "   vec3 reflection = normalize(reflect(lightDir, fNormal));\n"
        "   float spec = max(0.0,dot(eyeDir, reflection));\n"
        "   vec3 specular = pow(spec,specularExpMaterial) * specularMaterial.xyz;\n"
        "   out_color= vec4(ambient + diffuse + specular,1.0);\n"
        //"   out_color= vec4(ambient ,1.0);\n"
        //"    out_color= vec4(normal,1.0);\n"
        "}\n";
        return prepareShader(text, GL_FRAGMENT_SHADER);
    }

    ShaderProgram phong_shader_program(int dim) {

        auto v = simple_vertex_shader(dim);
        auto f = phong_fragment_shader();
        return linkShaderProgram(v,f);
    }
}}}

