#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/gl.h>
#include <vector>
#include "mtao/opengl/util.h"
namespace mtao { namespace opengl {
struct Texture: public bind_enabled<Texture> {
    public:
    Texture(GLint internalFormat, GLsizei w=-1, GLsizei h=-1, GLsizei d=-1);
    Texture(Texture&& other);
    Texture();
    ~Texture();
    GLenum optional() const { return type; }
    GLuint id() const { return m_id; }
    static void bind(GLuint id, GLenum type);
    static void release(GLuint id, GLenum type);
    void resize(GLsizei w=-1, GLsizei h=-1, GLsizei d=-1);
    void setData(const void* data, GLsizei w=-1, GLsizei h=-1,GLsizei d=-1);
    //void dump(const std::string& filename);
    private:

    GLsizei w,h,d;
    GLuint m_id= -1;
    GLint m_target;

    //GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D, GL_TEXTURE_1D_ARRAY,
    //GL_PROXY_TEXTURE_1D_ARRAY, GL_TEXTURE_RECTANGLE,
    //GL_PROXY_TEXTURE_RECTANGLE, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    //GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    //GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    //GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, or GL_PROXY_TEXTURE_CUBE_MAP
    GLenum type = GL_TEXTURE_2D;
    GLint internalFormat = GL_RED;
};
}}
#endif//TEXTURE_H
