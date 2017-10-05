#include "opengl/vao.h"
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr


namespace mtao { namespace opengl {
VAO::VAO() {
     glGenVertexArrays(1,&m_id);
     bind();
}

VAO::~VAO() {
    glDeleteVertexArrays(1,&m_id);
}

void VAO::bind() {
    glBindVertexArray(m_id);
}
void VAO::release() {
    glBindVertexArray(0);
}

AO::AO(GLint id): m_id(id) {}

auto AO::enableRAII() const -> AttributeObjectEnabled {
    return AttributeObjectEnabled(m_id);
}

AO::AttributeObjectEnabled::AttributeObjectEnabled(GLint id): m_id(id)  {
    glEnableVertexAttribArray(m_id);
}
AO::AttributeObjectEnabled::~AttributeObjectEnabled()  {
    glDisableVertexAttribArray(m_id);
}

void AO::enable() const {
    glEnableVertexAttribArray(m_id);
}
void AO::disable() const {
    glDisableVertexAttribArray(m_id);
}

void AO::setPointer( GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *offset) {
    glEnableVertexAttribArray(m_id);
    glVertexAttribPointer(m_id, size, type, normalized, stride, offset);
}
void AO::setPointerI(GLint size, GLenum type, GLsizei stride, const void *offset ) {
    glEnableVertexAttribArray(m_id);
    glVertexAttribIPointer(m_id, size, type, stride, offset);
}
void AO::setPointerL( GLint size, GLenum type, GLsizei stride, const void *offset ) {
    glEnableVertexAttribArray(m_id);
    glVertexAttribLPointer(m_id, size, type, stride, offset);
}


UO::UO(GLint id): m_id(id) {}

void UO::set1f(	GLfloat v0) const {
    glUniform1f(m_id,v0);
}

void UO::set2f(	GLfloat v0, GLfloat v1) const {
    glUniform2f(m_id,v0,v1);
}

void UO::set3f(	GLfloat v0, GLfloat v1, GLfloat v2) const {
    glUniform3f(m_id,v0,v1,v2);
}

void UO::set4f(	GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const {
    glUniform4f(m_id,v0,v1,v2,v3);
}

void UO::set1i(	GLint v0) const {
    glUniform1i(m_id,v0);
}

void UO::set2i(	GLint v0, GLint v1) const {
    glUniform2i(m_id,v0,v1);
}

void UO::set3i(	GLint v0, GLint v1, GLint v2) const {
    glUniform3i(m_id,v0,v1,v2);
}

void UO::set4i(	GLint v0, GLint v1, GLint v2, GLint v3) const {
    glUniform4i(m_id,v0,v1,v2,v3);
}

void UO::set1ui(GLuint v0) const {
    glUniform1ui(m_id,v0);
}

void UO::set2ui(GLuint v0, GLuint v1) const {
    glUniform2ui(m_id,v0,v1);
}

void UO::set3ui(GLuint v0, GLuint v1, GLuint v2) const {
    glUniform3ui(m_id,v0,v1,v2);
}

void UO::set4ui(GLuint v0, GLuint v1, GLuint v2, GLuint v3) const {
    glUniform4ui(m_id,v0,v1,v2,v3);
}




void UO::set1fv(	GLsizei count, const GLfloat *value) const {
    glUniform1fv(m_id,count,value);
}
void UO::set2fv(	GLsizei count, const GLfloat *value) const {
    glUniform2fv(m_id,count,value);
}
void UO::set3fv(	GLsizei count, const GLfloat *value) const {
    glUniform3fv(m_id,count,value);
}
void UO::set4fv(	GLsizei count, const GLfloat *value) const {
    glUniform4fv(m_id,count,value);
}

void UO::set1iv(	GLsizei count, const GLint *value) const {
    glUniform1iv(m_id,count,value);
}
void UO::set2iv(	GLsizei count, const GLint *value) const {
    glUniform2iv(m_id,count,value);
}
void UO::set3iv(	GLsizei count, const GLint *value) const {
    glUniform3iv(m_id,count,value);
}
void UO::set4iv(	GLsizei count, const GLint *value) const {
    glUniform4iv(m_id,count,value);
}

void UO::set1uiv(	GLsizei count, const GLuint *value) const {
    glUniform1uiv(m_id,count,value);
}
void UO::set2uiv(	GLsizei count, const GLuint *value) const {
    glUniform2uiv(m_id,count,value);
}
void UO::set3uiv(	GLsizei count, const GLuint *value) const {
    glUniform3uiv(m_id,count,value);
}
void UO::set4uiv(	GLsizei count, const GLuint *value) const {
    glUniform4uiv(m_id,count,value);
}


void UO::setMatrix2fv(	GLsizei count, GLboolean transpose, const GLfloat *value) const {
    glUniformMatrix2fv(m_id,count,transpose,value);
}
void UO::setMatrix3fv(	GLsizei count, GLboolean transpose, const GLfloat *value) const {
    glUniformMatrix3fv(m_id,count,transpose,value);
}
void UO::setMatrix4fv(	GLsizei count, GLboolean transpose, const GLfloat *value) const {
    glUniformMatrix4fv(m_id,count,transpose,value);
}

void UO::setMatrix2x3fv(	GLsizei count, GLboolean transpose, const GLfloat *value) const {
    glUniformMatrix2x3fv(m_id,count,transpose,value);
}
void UO::setMatrix3x2fv(	GLsizei count, GLboolean transpose, const GLfloat *value) const {
    glUniformMatrix3x2fv(m_id,count,transpose,value);
}
void UO::setMatrix2x4fv(	GLsizei count, GLboolean transpose, const GLfloat *value) const {
    glUniformMatrix2x4fv(m_id,count,transpose,value);
}
void UO::setMatrix4x2fv(	GLsizei count, GLboolean transpose, const GLfloat *value) const {
    glUniformMatrix4x2fv(m_id,count,transpose,value);
}
void UO::setMatrix3x4fv(	GLsizei count, GLboolean transpose, const GLfloat *value) const {
    glUniformMatrix3x4fv(m_id,count,transpose,value);
}
void UO::setMatrix4x3fv(	GLsizei count, GLboolean transpose, const GLfloat *value) const {
    glUniformMatrix4x3fv(m_id,count,transpose,value);
}




void UO::setVector(const glm::vec2& v) const {
    set2fv(1, glm::value_ptr(v));
}
void UO::setVector(const glm::vec3& v) const {
    set3fv(1, glm::value_ptr(v));
}
void UO::setVector(const glm::vec4& v) const {
    set4fv(1, glm::value_ptr(v));
}
void UO::setMatrix(const glm::mat2& m) const {
    setMatrix2fv(1, GL_FALSE, glm::value_ptr(m));
}
void UO::setMatrix(const glm::mat3& m) const {
    setMatrix3fv(1, GL_FALSE, glm::value_ptr(m));
}
void UO::setMatrix(const glm::mat4& m) const {
    setMatrix4fv(1, GL_FALSE, glm::value_ptr(m));
}




}}
