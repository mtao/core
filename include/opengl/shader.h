#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <string>
#include "vao.h"

namespace mtao { namespace opengl {
struct Shader {
    public:
        Shader(GLenum shader_type = GL_VERTEX_SHADER);
        ~Shader();
        void setSource(const GLchar** source);
        bool compile();
        bool compile(const GLchar** source);
        GLuint id() const { return m_id; }
    private:
        bool compilation_check();
    private:
        GLenum m_shader_type;
        GLuint m_id;
};

struct ShaderProgram {
    public:
        struct ShaderProgramEnabled {
            public:
                ShaderProgramEnabled(GLuint id);
                ~ShaderProgramEnabled();
        };
        ShaderProgram();
        ~ShaderProgram();
        void attach(GLuint shader);
        void attach(const Shader& shader);
        bool compile();
        GLuint id() const { return m_id; }
        ShaderProgramEnabled useRAII();
        void use();
        static void useNone();


        UO getUniform(const std::string& name) const;
        AO getAttrib(const std::string& name) const;
        GLint getUniformLocation(const std::string& name) const;
        GLint getAttribLocation(const std::string& name) const;

        template <typename... Args>
            UO setUniform(const std::string& name, Args&&... args) const {
                UO uo = getUniform(name);
                uo.set(std::forward<Args>(args)...);
                return std::move(uo);
            }
    private:
        bool compilation_check();
    private:
        GLuint m_id;
};
}}

#endif//SHADER_H
