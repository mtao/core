#ifndef VERTEX_ATTRIBUTE_H
#define VERTEX_ATTRIBUTE_H
#include <glad/glad.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include "opengl/util.h"


namespace mtao { namespace opengl {

    //vertex attribute object
    struct VAO: public bind_enabled<VAO> {
        public:
            VAO();
            ~VAO();
            using bind_enabled<VAO>::bind;
            using bind_enabled<VAO>::release;
            auto enableRAII() const { return make_binder(*this); }
            GLuint id() const { return m_id; }
            static void bind(GLuint id);
            static void release(GLuint id);

        private:
            GLuint m_id;

    };
    //vertex attribute pointer
    struct AO: public bind_enabled<AO> {
        public:
            AO(GLint id = GL_INVALID_VALUE);
            using bind_enabled<AO>::bind;
            using bind_enabled<AO>::release;

            auto enableRAII() const { return make_binder(*this); }
            void enable() const;
            void disable() const;
            GLuint id() const { return m_id; }
            static void bind(GLuint id);
            static void release(GLuint id);
            //glVertexAttribPointer
            //Allowed types:
            //GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT
            //GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE, GL_FIXED, GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV, GL_UNSIGNED_INT_10F_11F_11F_REV.
            void setPointer( GLint size=4, GLenum type=GL_FLOAT, GLboolean normalized=GL_FALSE, GLsizei stride=0, const void *offset=0);
            //glVertexAttribIPointer
            //Allowed types:
            //GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT
            void setPointerI(GLint size=4, GLenum type=GL_FLOAT, GLsizei stride=0, const void *offset=0);

            //glVertexAttribLPointer
            //Only allows: GL_DOUBLE
            void setPointerL(GLint size=4, GLenum type=GL_DOUBLE, GLsizei stride=0, const void *offset=0);
        private:
            GLint m_id;
    };
    //Uniform Pointer
    struct UO {
        public:
            UO(GLint id = GL_INVALID_VALUE);
            void set1f(GLfloat v0) const;
            void set2f(GLfloat v0, GLfloat v1) const;
            void set3f(GLfloat v0, GLfloat v1, GLfloat v2) const;
            void set4f(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const;
            void set1i(GLint v0) const;
            void set2i(GLint v0, GLint v1) const;
            void set3i(GLint v0, GLint v1, GLint v2) const;
            void set4i(GLint v0, GLint v1, GLint v2, GLint v3) const;
            void set1ui(GLuint v0) const;
            void set2ui(GLuint v0, GLuint v1) const;
            void set3ui(GLuint v0, GLuint v1, GLuint v2) const;
            void set4ui(GLuint v0, GLuint v1, GLuint v2, GLuint v3) const;

            void set(GLfloat v0) const {set1f(v0);}
            void set(GLfloat v0, GLfloat v1) const {set2f(v0,v1);}
            void set(GLfloat v0, GLfloat v1, GLfloat v2) const {set3f(v0,v1,v2);}
            void set(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const {set4f(v0,v1,v2,v3);}
            void set(GLint v0) const {set1i(v0);}
            void set(GLint v0, GLint v1) const {set2i(v0,v1);}
            void set(GLint v0, GLint v1, GLint v2) const {set3i(v0,v1,v2);}
            void set(GLint v0, GLint v1, GLint v2, GLint v3) const {set4i(v0,v1,v2,v3);}
            void set(GLuint v0) const {set1ui(v0);}
            void set(GLuint v0, GLuint v1) const {set2ui(v0,v1);}
            void set(GLuint v0, GLuint v1, GLuint v2) const {set3ui(v0,v1,v2);}
            void set(GLuint v0, GLuint v1, GLuint v2, GLuint v3) const {set4ui(v0,v1,v2,v3);}

            void set1fv(GLsizei count, const GLfloat *value) const;
            void set2fv(GLsizei count, const GLfloat *value) const;
            void set3fv(GLsizei count, const GLfloat *value) const;
            void set4fv(GLsizei count, const GLfloat *value) const;

            void set1iv(GLsizei count, const GLint *value) const;
            void set2iv(GLsizei count, const GLint *value) const;
            void set3iv(GLsizei count, const GLint *value) const;
            void set4iv(GLsizei count, const GLint *value) const;

            void set1uiv(GLsizei count, const GLuint *value) const;
            void set2uiv(GLsizei count, const GLuint *value) const;
            void set3uiv(GLsizei count, const GLuint *value) const;
            void set4uiv(GLsizei count, const GLuint *value) const;


            void setMatrix2fv(GLsizei count, GLboolean transpose, const GLfloat *value) const;
            void setMatrix3fv(GLsizei count, GLboolean transpose, const GLfloat *value) const;
            void setMatrix4fv(GLsizei count, GLboolean transpose, const GLfloat *value) const;

            void setMatrix2x3fv(GLsizei count, GLboolean transpose, const GLfloat *value) const;
            void setMatrix3x2fv(GLsizei count, GLboolean transpose, const GLfloat *value) const;
            void setMatrix2x4fv(GLsizei count, GLboolean transpose, const GLfloat *value) const;
            void setMatrix4x2fv(GLsizei count, GLboolean transpose, const GLfloat *value) const;
            void setMatrix3x4fv(GLsizei count, GLboolean transpose, const GLfloat *value) const;
            void setMatrix4x3fv(GLsizei count, GLboolean transpose, const GLfloat *value) const;


            void setVector(const glm::vec2& v) const;
            void setVector(const glm::vec3& v) const;
            void setVector(const glm::vec4& v) const;
            void setMatrix(const glm::mat2& m) const;
            void setMatrix(const glm::mat3& m) const;
            void setMatrix(const glm::mat4& m) const;
    private:
            GLint m_id;
    };
}}

#endif//VERTEX_ATTRIBUTE_H
