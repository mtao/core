#pragma once

#include "mtao/opengl/renderers/mesh.h"
#include <Eigen/Geometry>

namespace mtao {
namespace opengl {
    namespace renderers {


        class AxisRenderer : public MeshRenderer {
          public:
            AxisRenderer(int D);
            ~AxisRenderer();

            void show();
            void hide();
            void toggle();
            bool visible() const { return m_visible; }
            void render() const override;

          private:
            bool m_visible = true;
            float m_length = .1;
        };

    }// namespace renderers
}// namespace opengl
}// namespace mtao
