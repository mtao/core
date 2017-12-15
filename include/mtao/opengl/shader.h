#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <string>
#include <memory>
#include "mtao/opengl/util.h"
#include "mtao/opengl/vao.h"
#include "mtao/type_utils.h"


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

    struct ShaderProgram: public bind_enabled<ShaderProgram> {
        public:
            ShaderProgram();
            ShaderProgram(ShaderProgram&& other);
            ~ShaderProgram();
            ShaderProgram& operator=(ShaderProgram&& other);
            void attach(GLuint shader);
            void attach(const Shader& shader);
            bool compile();
            GLuint id() const { return m_id; }
            auto useRAII() {return make_binder(*this);}
            void use();
            void bind();
            void release();
            static void bind(GLuint);
            static void release(GLuint);


            UO getUniform(const std::string& name) const;
            AO getAttrib(const std::string& name) const;
            GLint getUniformLocation(const std::string& name) const;
            GLint getAttribLocation(const std::string& name) const;

            template <typename... Args>
                UO setUniform(const std::string& name, Args&&... args) const {
                    UO uo = getUniform(name);
                    uo.set(std::forward<Args>(args)...);
                    return uo;
                }
        private:
            bool compilation_check();
        private:
            GLuint m_id;
    };


    Shader prepareShader(const char* data, GLenum type);
    Shader prepareShader(const std::tuple<const char*, GLenum >& t);


    ShaderProgram prepareShaders(const char* vdata, const char* fdata, const char* geo = nullptr);
    template <typename... Shaders>
        ShaderProgram linkShaderProgram(const Shaders&... shaders) {
            static_assert((std::is_same<Shaders, Shader>::value && ...));
            ShaderProgram program;
            (program.attach(shaders), ...);
            program.compile();
            return program;
        }
    template <typename... ShaderData>
    ShaderProgram prepareShaders(const std::tuple<ShaderData,GLenum>&... shaders) {
            static_assert((std::is_convertible<ShaderData, const char*>::value && ...));

            return linkShaderProgram(prepareShader(shaders)...);
    }

}}

#endif//SHADER_H
