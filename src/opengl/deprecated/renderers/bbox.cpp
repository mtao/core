#include "mtao/opengl/renderers/bbox.h"
namespace mtao {
namespace opengl {
    namespace renderers {

        constexpr static GLuint faces2[2 * 4] = {
            0,
            1,
            2,
            3,
            0,
            2,
            1,
            3
        };
        constexpr static GLuint faces3[2 * 12] = {
            0,
            1,
            2,
            3,
            4,
            5,
            6,
            7,
            0,
            4,
            1,
            5,
            2,
            6,
            3,
            7,
            0,
            2,
            1,
            3,
            4,
            6,
            5,
            7,
        };

        BBoxRendererBase::BBoxRendererBase(int D, const GLuint *E) : MeshRenderer(D) {
            hide_all();
            set_edge_style(EdgeStyle::Mesh);
            line_width() = 5;
            edge_color() = glm::vec4(.7, .7, .7, 1);

            if (D == 2) {
                setEdges(Eigen::Map<const MatrixXui>(E, 2, 4));
            } else if (D == 3) {
                setEdges(Eigen::Map<const MatrixXui>(E, 2, 12));
            }
        }

        void BBoxRendererBase::show() { m_visible = true; }
        void BBoxRendererBase::hide() { m_visible = false; }
        void BBoxRendererBase::toggle() { m_visible ^= true; }

        void BBoxRendererBase::render() const {
            if (visible()) {
                glEnable(GL_DEPTH_TEST);
                render_edges();
            }
        }
        template<>
        void BBoxRenderer<2>::set(const BBox &bbox) {
            m_bbox = bbox;
            MatrixXgf V(2, 4);
            V.col(0) = m_bbox.corner(BBox::BottomLeft);
            V.col(1) = m_bbox.corner(BBox::BottomRight);
            V.col(2) = m_bbox.corner(BBox::TopLeft);
            V.col(3) = m_bbox.corner(BBox::TopRight);
            setVertices(V, false);
        }
        template<>
        void BBoxRenderer<3>::set(const BBox &bbox) {
            m_bbox = bbox;
            MatrixXgf V(3, 8);
            V.col(0) = m_bbox.corner(BBox::BottomLeftFloor);
            V.col(1) = m_bbox.corner(BBox::BottomRightFloor);
            V.col(2) = m_bbox.corner(BBox::TopLeftFloor);
            V.col(3) = m_bbox.corner(BBox::TopRightFloor);
            V.col(4) = m_bbox.corner(BBox::BottomLeftCeil);
            V.col(5) = m_bbox.corner(BBox::BottomRightCeil);
            V.col(6) = m_bbox.corner(BBox::TopLeftCeil);
            V.col(7) = m_bbox.corner(BBox::TopRightCeil);

            setVertices(V, false);
        }

        template<>
        BBoxRenderer<2>::BBoxRenderer() : BBoxRendererBase(2, faces2) {}
        template<>
        BBoxRenderer<3>::BBoxRenderer() : BBoxRendererBase(3, faces3) {}
    }// namespace renderers
}// namespace opengl
}// namespace mtao
