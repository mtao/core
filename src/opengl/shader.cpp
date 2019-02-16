#include "mtao/opengl/opengl_loader.hpp"
#include "mtao/opengl/shader.h"
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
                logging::error() << "Vertex"; break;
            case GL_FRAGMENT_SHADER: 
                logging::error() << "Fragment"; break;
            case GL_GEOMETRY_SHADER: 
                logging::error() << "Geometry"; break;
            default:
                logging::error() << "Unknown"; break;

        }
        logging::error() << " shader error:";


        std::vector<GLchar> log(logSize);
        glGetShaderInfoLog(m_id, logSize, &logSize, log.data());
        logging::error() << log.data();
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
ShaderProgram::ShaderProgram(ShaderProgram&& other): m_id(other.id()) {
    other.m_id = GL_INVALID_VALUE;

}
ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) {
    m_id = other.id();
    other.m_id = GL_INVALID_VALUE;
    return *this;
}
ShaderProgram::~ShaderProgram() {
    if(id() != GL_INVALID_VALUE) {
        glDeleteProgram(m_id);
    }
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

void ShaderProgram::release() {
    release(m_id);
}
void ShaderProgram::bind(GLuint id) {
    glUseProgram(id);
}
void ShaderProgram::release(GLuint id) {
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
        logging::error() << log.data();
        return false;
        return false;
    }
    return true;

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


Shader prepareShader(const char* data, GLenum type) {
    Shader shader(type);
    shader.compile(&data);
    return shader;
}
Shader prepareShader(const std::tuple<const char*, GLenum >& t) {
    return prepareShader(std::get<0>(t),std::get<1>(t));
}


ShaderProgram prepareShaders(const char* vdata, const char* fdata, const char* geo) {

    auto vs = prepareShader(vdata,GL_VERTEX_SHADER);
    auto fs = prepareShader(fdata,GL_FRAGMENT_SHADER);

    if(geo) {
        auto gs = prepareShader(geo,GL_GEOMETRY_SHADER);
        return linkShaderProgram(vs,fs,gs);
    } else {
        return linkShaderProgram(vs,fs);
    }


}

}}
