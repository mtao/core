#include "opengl/bo.h"
#include <cassert>
#include <GLFW/glfw3.h>
#include <iostream>

namespace mtao { namespace opengl {

static GLsizei sizeof_glenum(GLenum type_enum) {
    switch(type_enum) {
        case GL_UNSIGNED_BYTE:
            return sizeof(GLubyte);
        case GL_UNSIGNED_SHORT:
            return sizeof(GLushort);
        case GL_UNSIGNED_INT:
            return sizeof(GLuint);
        case GL_FLOAT:
            return sizeof(GLfloat);
        default: return 0;
    }
}

BO::BO(GLenum target, GLenum usage, GLenum type): m_target(target), m_usage(usage), m_type(type) {
    glGenBuffers(1, &m_id);
}

void BO::bind() {
        glBindBuffer(m_target, m_id);
}


void BO::setData(const GLvoid* data, GLsizeiptr size) {
    bind();
    glBufferData(m_target, size, data, m_usage);
}

GLint BO::size() const {
    int s;  glGetBufferParameteriv(target(), GL_BUFFER_SIZE, &s);
    s = s/sizeof_glenum(type());
    return s;
}

VBO::VBO(GLenum mode , GLenum usage , GLenum type ): BO(GL_ARRAY_BUFFER,usage,type), m_mode(mode) {
}

void VBO::drawArrays(GLenum mode) {
    if(mode == GL_INVALID_ENUM) {
        mode = m_mode;
    }
    bind();
    assert(target() == GL_ARRAY_BUFFER);
    glDrawArrays(mode,first, size());
}


BO::~BO() {
    glDeleteBuffers(1,&m_id);
}



IBO::IBO(GLenum mode, GLenum usage, GLenum type): BO(GL_ELEMENT_ARRAY_BUFFER, usage,type), m_mode(mode) {}


void IBO::drawElements() {
    bind();
    assert(target() == GL_ELEMENT_ARRAY_BUFFER);
    glDrawElements(m_mode, size(), type(), indices);
}
}}

