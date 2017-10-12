#ifndef BUFFER_H
#define BUFFER_H

#include <glad/glad.h>
namespace mtao { namespace opengl {


struct BO {
    public:
        BO(GLenum m_target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);
        ~BO();
        BO(const BO& other) = delete;
        BO(BO&& other) = default;
        BO& operator=(const BO& other) = delete;
        BO& operator=(BO&& other) = default;


        void bind();
        void setData(const GLvoid* data, GLsizeiptr size);
        GLenum target() const { return m_target; }
        GLenum usage() const { return m_usage; }
    private:
        GLuint m_id;
        GLenum m_target = GL_ARRAY_BUFFER;
        GLenum m_usage = GL_STATIC_DRAW;
};

struct IBO: public BO {
    public:
        IBO(GLenum mode = GL_TRIANGLES, GLenum usage = GL_STATIC_DRAW);
        void drawElements();
    private:
        GLenum m_mode;
        GLenum m_type = GL_UNSIGNED_INT;
        const GLvoid * indices = 0;
};
}}


#endif//BUFFER_H

