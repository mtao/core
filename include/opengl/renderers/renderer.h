#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>
#include <list>
#include <atomic>
#include "opengl/shader.h"

namespace mtao { namespace opengl { namespace renderers {

class Renderer {
    public:
        Renderer();
        virtual void render() const = 0;
        virtual void imgui_interface();
        void set_mvp(const glm::mat4& mv, const glm::mat4&p);
        void set_mvp(const glm::mat4& mvp);
        virtual std::list<ShaderProgram*> mvp_programs() const { return {};}

        VAO& vao() { return *m_vertex_attributes; }
        const VAO& vao() const { return *m_vertex_attributes; }

        static void save_frame(const std::string& filename, int w, int h) ;


    private:
        std::unique_ptr<VAO> m_vertex_attributes;

};
}}}
#endif//RENDERER_H
