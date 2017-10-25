#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>
#include <list>
#include "opengl/shader.h"

namespace mtao { namespace opengl { namespace renderers {

class Renderer {
    public:
        virtual void render() const = 0;
        virtual void imgui_interface();
        void set_mvp(const glm::mat4& mv, const glm::mat4&p);
        void set_mvp(const glm::mat4& mvp);
        virtual std::list<ShaderProgram*> mvp_programs() const { return {};}

    private:
        
};
}}}
#endif//RENDERER_H
