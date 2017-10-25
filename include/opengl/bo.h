#ifndef BUFFER_H
#define BUFFER_H

#include <glad/glad.h>
namespace mtao { namespace opengl {


struct BO {
    public:
        BO( GLenum m_target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW, GLenum type = GL_FLOAT);
        ~BO();
        BO(const BO& other) = delete;
        BO(BO&& other) = default;
        BO& operator=(const BO& other) = delete;
        BO& operator=(BO&& other) = default;


        void bind();
        void setData(const GLvoid* data, GLsizeiptr size);
        GLenum target() const { return m_target; }
        GLenum usage() const { return m_usage; }
        GLenum type() const { return m_type; }
    private:
        GLuint m_id;
        GLenum m_target = GL_ARRAY_BUFFER;
        GLenum m_usage = GL_STATIC_DRAW;
        GLenum m_type = GL_FLOAT;
};
struct VBO: public BO {
    public:
        VBO(GLenum mode = GL_POINTS, GLenum usage = GL_STATIC_DRAW, GLenum type = GL_FLOAT);
        void drawArrays(GLenum mode =GL_INVALID_ENUM );
    private:
        GLenum m_mode;
        GLint first = 0;
};

struct IBO: public BO {
    public:
        IBO(GLenum mode = GL_TRIANGLES, GLenum usage = GL_STATIC_DRAW, GLenum type = GL_UNSIGNED_INT);
        void drawElements();
    private:
        GLenum m_mode;
        const GLvoid * indices = 0;
};
}}


#endif//BUFFER_H

