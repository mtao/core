#include "opengl/shaders.h"
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
        "   vec4 mvpos = MV * vec4(fPos,1);\n"
        "   mvpos = vec4(fPos,1);\n"
        "   vec3 lightDir = normalize(lightPos - mvpos.xyz/mvpos.w);\n"

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
}}}
