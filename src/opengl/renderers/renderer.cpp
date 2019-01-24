#include "mtao/opengl/renderers/renderer.h"

namespace mtao { namespace opengl { namespace renderers {

    Renderer::Renderer() {

        m_vertex_attributes = std::make_unique<VAO>();
    }
    void Renderer::imgui_interface(const std::string& name) {

    }
    void Renderer::set_mvp(const glm::mat4& mv, const glm::mat4&p) {
        for(auto&& prg: mvp_programs()) {
            if(prg) {
                auto active = prg->useRAII();
                prg->getUniform("MV").setMatrix(mv);
                prg->getUniform("P").setMatrix(p);
            }
        }

        set_mvp(p*mv);


    }
    void Renderer::set_mvp(const glm::mat4& mvp) {

        for(auto&& p: mvp_programs()) {
            if(p) {
                auto active = p->useRAII();
                p->getUniform("MVP").setMatrix(mvp);
            }
        }
    }


}}}
