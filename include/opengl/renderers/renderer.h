#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>

namespace mtao { namespace opengl { namespace renderers {

class Renderer {
    public:
        virtual void render() const = 0;
    private:
        
};
}}}
#endif//RENDERER_H
