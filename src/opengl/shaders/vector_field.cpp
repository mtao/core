#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Corrade/Containers/Reference.h>


#include "mtao/opengl/shaders/vector_field.hpp"

using namespace Corrade;
using namespace Magnum;

namespace mtao::opengl {

    VectorFieldShader::VectorFieldShader(Magnum::UnsignedInt colorMode): _colorMode(colorMode) {
        Utility::Resource rs("MtaoShaders");

        GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex},
            geom{GL::Version::GL330, GL::Shader::Type::Geometry},
            frag{GL::Version::GL330, GL::Shader::Type::Fragment};

        if(_colorMode == PerVectorColors) {
            constexpr static char s[] = "#define PER_VERTEX_COLOR\n";
            vert.addSource(s);
            geom.addSource(s);
            frag.addSource(s);

        }
        vert.addSource(rs.get("vector_field.vert"));
        geom.addSource(rs.get("vector_field.geom"));
        frag.addSource(rs.get("vector_field.frag"));
        CORRADE_INTERNAL_ASSERT(GL::Shader::compile({vert,geom,frag}));
        attachShaders({vert,geom,frag});
        CORRADE_INTERNAL_ASSERT(link());
        if(_colorMode == UniformColor) {
            _colorUniform = uniformLocation("color");
        }
        _transformationMatrixUniform = uniformLocation("transformationMatrix");
        _projectionMatrixUniform = uniformLocation("projectionMatrix");
        _normalMatrixUniform = uniformLocation("normalMatrix");
    }
}
    template <>
        void Drawable<VectorFieldShader>::set_buffers() {
            std::string name = name_;
            if(name.empty()) {
                name = "Vertex Color Shader";
            }
            if(ImGui::TreeNode(name.c_str())) {
                ImGui::Checkbox("Visible", &visible);
                ImGui::TreePop();
            }
        }
