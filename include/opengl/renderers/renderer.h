#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>

namespace mtao { namespace opengl { namespace renderers {

class Renderer {
    public:
        virtual void render() const = 0;
        virtual void imgui_interface();
    private:
        
};
}}}
#endif//RENDERER_H
