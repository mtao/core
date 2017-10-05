#include "opengl/bo.h"
#include <cassert>
#include <GLFW/glfw3.h>

namespace mtao { namespace opengl {


BO::BO(GLenum target, GLenum usage): m_target(target), m_usage(usage) {
    glGenBuffers(1, &m_id);
}

void BO::bind() {
        glBindBuffer(m_target, m_id);
}


void BO::setData(const GLvoid* data, GLsizeiptr size) {
    glBufferData(m_target, size, data, m_usage);
}



BO::~BO() {
    glDeleteBuffers(1,&m_id);
}



IBO::IBO(GLenum mode, GLenum usage): BO(GL_ELEMENT_ARRAY_BUFFER, usage), m_mode(mode) {}

static GLsizei sizeof_glenum(GLenum type_enum) {
    switch(type_enum) {
        case GL_UNSIGNED_BYTE:
            return sizeof(GLubyte);
        case GL_UNSIGNED_SHORT:
            return sizeof(GLushort);
        case GL_UNSIGNED_INT:
            return sizeof(GLuint);
        default: return 0;
    }
}

void IBO::drawElements() {
    bind();
    assert(target() == GL_ELEMENT_ARRAY_BUFFER);
    int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(m_mode, size/sizeof_glenum(m_type), m_type, 0);
}
}}

