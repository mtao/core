#pragma once

#include "mtao/opengl/renderers/mesh.h"
#include <Eigen/Geometry>

namespace mtao { namespace opengl { namespace renderers {

template <int D>
class BBoxRenderer;

using BBoxRenderer2 = BBoxRenderer<2>;
using BBoxRenderer3 = BBoxRenderer<3>;
class BBoxRendererBase: public MeshRenderer {
    public:
        template <int D>
            friend class BBoxRenderer;

        void show();
        void hide();
        void toggle();
        bool visible() const { return m_visible; }
        void render() const override;

    protected:
        BBoxRendererBase(int D, const GLuint* edges);
    private:
        bool m_visible = true;
};
template <int D>
class BBoxRenderer: public BBoxRendererBase {

    public:
        using Vec = Eigen::Matrix<float,D,1>;
        using BBox = Eigen::AlignedBox<float,D>;
        BBoxRenderer();
        BBoxRenderer(const BBox& bbox): BBoxRenderer(){
            set(bbox);
        }
        BBoxRenderer(const Vec& min, const Vec& max): BBoxRenderer() {
            set(min,max);
        }
        void set(const BBox& bbox);
        void set(const Vec& min, const Vec& max) {set(BBox(min,max));}

    private:
        BBox m_bbox = BBox(Vec::Zero(),Vec::Ones());;
};

template <>
BBoxRenderer<2>::BBoxRenderer(const BBox& bbox);
template <>
BBoxRenderer<3>::BBoxRenderer(const BBox& bbox);
template <>
BBoxRenderer<2>::BBoxRenderer();
template <>
BBoxRenderer<3>::BBoxRenderer();

}}}
