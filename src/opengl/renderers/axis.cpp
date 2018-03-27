#include "mtao/opengl/renderers/axis.h"
namespace mtao { namespace opengl { namespace renderers {


AxisRenderer::AxisRenderer(int D): MeshRenderer(D) {
    hide_all();
    set_edge_style(EdgeStyle::Color);
    line_width() = 5;
    Eigen::MatrixXf I(D,2*D);
    Eigen::MatrixXf C(3,2*D);
    if(D == 2) {
        I << 
            0,1,0,0,
            0,0,0,1,
        C << 
            1,1,0,0,
            0,0,1,1,
            0,0,0,0;
    } else if(D == 3) {
        I << 
            0,1,0,0,0,0,
            0,0,0,1,0,0,
            0,0,0,0,0,1;
        C << 
            1,1,0,0,0,0,
            0,0,1,1,0,0,
            0,0,0,0,1,1;
    }
    I *= m_length;
    setVertices(I,false);
    setColor(C);

    constexpr static GLuint faces[2*3] = {
        0,1,
        2,3,
        4,5,
    };
    setEdges(Eigen::Map<const MatrixXui>(faces,2,D));

}

void AxisRenderer::show() { m_visible = true; }
void AxisRenderer::hide() { m_visible = false; }
void AxisRenderer::toggle() { m_visible ^= true; }

void AxisRenderer::render() const {
    if(visible()) {
        glDisable(GL_DEPTH_TEST);
        render_edges();

    }
}
}}}
