#include "opengl/renderers/renderer.h"
#include <vector>
#include <png++/png.hpp>

namespace mtao { namespace opengl { namespace renderers {

    Renderer::Renderer() {

        m_vertex_attributes = std::make_unique<VAO>();
    }
    void Renderer::imgui_interface() {

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

    void Renderer::save_frame(const std::string& filename, int w, int h) {
        std::vector<unsigned char> data(4*w*h);
        glReadBuffer(GL_BACK);
        glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE, data.data());
        png::image<png::rgba_pixel> image(w,h);

        for (png::uint_32 y = 0; y < image.get_height(); ++y)
        {
            for (png::uint_32 x = 0; x < image.get_width(); ++x)
            {
                size_t o = 4*(x*w+h);
                image[y][x] = png::rgba_pixel(
                        data[o+0],
                        data[o+1],
                        data[o+2],
                        data[o+3]
                        );
            }
        }

        image.write(filename);
    }

}}}
