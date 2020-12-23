#include "mtao/opengl/shaders/vector_field.hpp"

#include <Magnum/GL/Renderer.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <imgui.h>

#include <iostream>

using namespace Corrade;
using namespace Magnum;
void initializer();
namespace mtao::opengl {

template <>
VectorFieldShader<2>::VectorFieldShader(Magnum::UnsignedInt colorMode)
    : _colorMode(colorMode) {
    initialize();
}
template <>
VectorFieldShader<3>::VectorFieldShader(Magnum::UnsignedInt colorMode)
    : _colorMode(colorMode) {
    initialize();
}
template <int D>
void VectorFieldShader<D>::initialize() {
    initializer();
    Utility::Resource rs("MtaoShaders");

    GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex},
        geom{GL::Version::GL330, GL::Shader::Type::Geometry},
        frag{GL::Version::GL330, GL::Shader::Type::Fragment};

    if (_colorMode == PerVectorColors) {
        constexpr static char s[] = "#define PER_VERTEX_COLOR\n";
        vert.addSource(s);
        geom.addSource(s);
        frag.addSource(s);
    }
    if constexpr (D == 2) {
        constexpr static char s[] = "#define TWO_DIMENSIONS\n";
        vert.addSource(s);
    }
    vert.addSource(rs.get("vector_field.vert"));
    geom.addSource(rs.get("vector_field.geom"));
    frag.addSource(rs.get("vector_field.frag"));
    CORRADE_INTERNAL_ASSERT(GL::Shader::compile({vert, geom, frag}));
    attachShaders({vert, geom, frag});
    CORRADE_INTERNAL_ASSERT(link());
    // glUseProgram(id());
    // GLchar buffer[2048];
    // GLsizei size;
    // glGetShaderSource(vert.id(), sizeof(buffer), &size, buffer);
    // std::cout << buffer << std::endl;
    _scaleUniform = uniformLocation("scale");
    if (_colorMode == UniformColor) {
        _colorUniform = uniformLocation("color");
    }
    _transformationProjectionMatrixUniform =
        uniformLocation("transformationProjectionMatrix");

    GLint params;
    GLuint indices;
    const char* name[1] = {"transformationProjectionMatrix"};

    glGetUniformIndices(id(), 1, name, &indices);

    glGetActiveUniformsiv(id(), 1, &indices, GL_UNIFORM_TYPE, &params);
    bindAttributeLocation(Vector::Location, "direction");
    //_transformationMatrixUniform = uniformLocation("transformationMatrix");
    //_projectionMatrixUniform = uniformLocation("projectionMatrix");
    //_normalMatrixUniform = uniformLocation("normalMatrix");
}
namespace internal {
template <typename Drawable>
void vfgui(Drawable& d, const std::string& name_) {
    std::string name = name_;
    if (name.empty()) {
        name = "Vector Field Shader";
    }
    if (ImGui::TreeNode(name.c_str())) {
        bool vis = d.is_visible();
        ImGui::Checkbox("Visible", &vis);
        d.set_visibility(vis);
        ImGui::ColorEdit4("Color", d.data().color.data());
        ImGui::InputFloat("Scale", &d.data().scale);
        ImGui::TreePop();
    }
}
template <typename Drawable>
void vfguim(Drawable& d, const std::string& name_) {
    std::string name = name_;
    if (name.empty()) {
        name = "Vector Field Shader";
    }
    if (ImGui::TreeNode(name.c_str())) {
        bool vis = d.is_visible();
        ImGui::Checkbox("Visible", &vis);
        d.set_visibility(vis);
        ImGui::ColorEdit4("Color", d.data().color.data());
        ImGui::InputFloat("Scale", &d.data().scale);
        ImGui::InputFloat("Line Width", &d.line_width);
        ImGui::TreePop();
    }
}
}  // namespace internal
template <>
void DrawableBase<VectorFieldShader<2>>::gui(const std::string& name_) {
    internal::vfgui(*this, name_);
}
template <>
void DrawableBase<VectorFieldShader<3>>::gui(const std::string& name_) {
    internal::vfgui(*this, name_);
}
template <>
void MeshDrawable<VectorFieldShader<2>>::gui(const std::string& name_) {
    internal::vfguim(*this, name_);
}
template <>
void MeshDrawable<VectorFieldShader<3>>::gui(const std::string& name_) {
    internal::vfguim(*this, name_);
}
template <>
void MeshDrawable<VectorFieldShader<2>>::set_buffers() {
    _mesh.addVertexBuffer(_mesh.vertex_buffer, 0,
                          VectorFieldShader<2>::Position{});
    _mesh.addVertexBuffer(_mesh.vfield_buffer, 0,
                          VectorFieldShader<2>::Vector{});
    if (shader().colorMode() == VectorFieldShader<2>::PerVectorColors) {
        // shader().setColor(_data.color);
    } else {
        _mesh.addVertexBuffer(_mesh.color_buffer, 0,
                              VectorFieldShader<2>::Color4{});
    }
    shader().setScale(data().scale).setColor(data().color);
    activate_line_width();


}
template <>
void MeshDrawable<VectorFieldShader<3>>::set_buffers() {
    _mesh.addVertexBuffer(_mesh.vertex_buffer, 0,
                          VectorFieldShader<3>::Position{});
    _mesh.addVertexBuffer(_mesh.vfield_buffer, 0,
                          VectorFieldShader<3>::Vector{});
    if (shader().colorMode() == VectorFieldShader<3>::PerVectorColors) {
        // shader().setColor(_data.color);
    } else {
        _mesh.addVertexBuffer(_mesh.color_buffer, 0,
                              VectorFieldShader<3>::Color4{});
    }
    shader().setScale(data().scale).setColor(data().color);
    activate_line_width();
}
}  // namespace mtao::opengl
