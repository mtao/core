#include "opengl/shader.h"
#include <vector>
#include <iostream>


namespace mtao { namespace opengl {


Shader::Shader(GLenum shader_type): m_shader_type(shader_type) {
    m_id = glCreateShader(shader_type);
}
Shader::~Shader() {
    glDeleteShader(m_id);
}

bool Shader::compilation_check() {
    GLint success = 0;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        GLint logSize = 0;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &logSize);
        switch(m_shader_type) {
            case GL_VERTEX_SHADER: 
                std::cout << "Vertex"; break;
            case GL_FRAGMENT_SHADER: 
                std::cout << "Fragment"; break;
            case GL_GEOMETRY_SHADER: 
                std::cout << "Geometry"; break;
            default:
                std::cout << "Unknown"; break;

        }
        std::cout << " shader error:" << std::endl;


        std::vector<GLchar> log(logSize);
        glGetShaderInfoLog(m_id, logSize, &logSize, log.data());
        std::cout << log.data() << std::endl;
        return false;
    }
    return true;
}

bool Shader::compile() {
    glCompileShader(m_id);
    return compilation_check();
}

void Shader::setSource(const GLchar** source) {
    glShaderSource(m_id, 1,source, nullptr);
}
bool Shader::compile(const GLchar** source) {
    setSource(source);
    return compile();
}


ShaderProgram::ShaderProgram() {
    m_id = glCreateProgram();
}
ShaderProgram::~ShaderProgram() {
    glDeleteProgram(m_id);
}

void ShaderProgram::attach(GLuint shader)  {
    glAttachShader(m_id, shader);
}
void ShaderProgram::attach(const Shader& shader) {
    attach(shader.id());
}
bool ShaderProgram::compile() {
    glLinkProgram(m_id);
    return compilation_check();
}

void ShaderProgram::use()  {
    glUseProgram(m_id);
}

auto ShaderProgram::useRAII() -> ShaderProgramEnabled {
    return ShaderProgramEnabled(id());
}
void ShaderProgram::useNone() {
    glUseProgram(0);
}
bool ShaderProgram::compilation_check() {
    GLint isLinked = 0;
    glGetProgramiv(m_id, GL_LINK_STATUS, &isLinked);
    if(isLinked == GL_FALSE) {
        GLint logSize;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &logSize);


        std::vector<GLchar> log(logSize);
        glGetProgramInfoLog(m_id, logSize, &logSize, log.data());
        std::cout << log.data() << std::endl;
        return false;
        return false;
    }
    return true;

}



ShaderProgram::ShaderProgramEnabled::ShaderProgramEnabled(GLuint id) {
    glUseProgram(id);
}
ShaderProgram::ShaderProgramEnabled::~ShaderProgramEnabled() {
    glUseProgram(0);
}
GLint ShaderProgram::getUniformLocation(const std::string& name) const {
    return glGetUniformLocation(id(), name.c_str());
}
GLint ShaderProgram::getAttribLocation(const std::string& name) const {
    return glGetAttribLocation(id(), name.c_str());
}

UO ShaderProgram::getUniform(const std::string& name) const {
    return UO(getUniformLocation(name));
}
AO ShaderProgram::getAttrib(const std::string& name) const {
    return AO(getAttribLocation(name));
}

}}
