#include "opengl/renderers/renderer.h"

namespace mtao { namespace opengl { namespace renderers {

    void Renderer::imgui_interface() {

    }
    void Renderer::set_mvp(const glm::mat4& mv, const glm::mat4&p) {
        for(auto&& prg: mvp_programs()) {
            auto active = prg->useRAII();
            prg->getUniform("MV").setMatrix(mv);
            prg->getUniform("P").setMatrix(p);
        }

        set_mvp(p*mv);


    }
    void Renderer::set_mvp(const glm::mat4& mvp) {

        for(auto&& p: mvp_programs()) {
            auto active = p->useRAII();
            p->getUniform("MVP").setMatrix(mvp);
        }
    }

}}}
