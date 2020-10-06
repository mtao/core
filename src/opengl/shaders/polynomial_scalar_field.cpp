
#include "mtao/opengl/shaders/polynomial_scalar_field.hpp"

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <colormap/colormap.h>
#include <fmt/format.h>
#include <imgui.h>

#include <iostream>

using namespace Corrade;
using namespace Magnum;
void initializer();

namespace mtao::opengl {

template <>
PolynomialScalarFieldShader<2>::PolynomialScalarFieldShader(
    Magnum::UnsignedInt colorMode)
    : _colorMap(colorMode) {
    initialize();
}
template <>
PolynomialScalarFieldShader<3>::PolynomialScalarFieldShader(
    Magnum::UnsignedInt colorMode)
    : _colorMap(colorMode) {
    initialize();
}
template <int D>
void PolynomialScalarFieldShader<D>::initialize() {
    initializer();
    Utility::Resource rs("MtaoShaders");

    GL::Shader vert{GL::Version::GL430, GL::Shader::Type::Vertex},
        frag{GL::Version::GL430, GL::Shader::Type::Fragment};

    if constexpr (D == 2) {
        constexpr static char s[] = "#define TWO_DIMENSIONS\n";
        vert.addSource(s);
        frag.addSource(s);
    }
    vert.addSource(rs.get("polynomial_scalar_field.vert"));
    frag.addSource(rs.get("polynomial.glsl"));
    switch (_colorMap) {
        case Parula:
            frag.addSource(colormap::MATLAB::Parula().getSource());
            break;
        case Jet:
            frag.addSource(colormap::MATLAB::Jet().getSource());
            break;
        case Waves:
            frag.addSource(colormap::IDL::Waves().getSource());
            break;
    }
    frag.addSource(rs.get("polynomial_scalar_field.frag"));
    CORRADE_INTERNAL_ASSERT(GL::Shader::compile({vert, frag}));
    attachShaders({vert, frag});
    CORRADE_INTERNAL_ASSERT(link());
    // glUseProgram(id());
    // GLchar buffer[2048];
    // GLsizei size;
    // glGetShaderSource(vert.id(), sizeof(buffer), &size, buffer);
    // std::cout << buffer << std::endl;
    _degree = uniformLocation("polynomial_coefficients.degree");
    _constant = uniformLocation("polynomial_coefficients.constant");
    _linear = uniformLocation("polynomial_coefficients.linear");
    _quadratic = uniformLocation("polynomial_coefficients.quadratic");
    _cubic = uniformLocation("polynomial_coefficients.cubic");
    _scale = uniformLocation("polynomial_coefficients.scale");
    _center = uniformLocation("polynomial_coefficients.center");
    _colormap_scale = uniformLocation("colormap_scale");
    _colormap_shift = uniformLocation("colormap_shift");
    _transformationProjectionMatrixUniform =
        uniformLocation("transformationProjectionMatrix");

    GLint params;
    GLuint indices;
    const char* name[1] = {"transformationProjectionMatrix"};

    glGetUniformIndices(id(), 1, name, &indices);

    glGetActiveUniformsiv(id(), 1, &indices, GL_UNIFORM_TYPE, &params);
    //_transformationMatrixUniform = uniformLocation("transformationMatrix");
    //_projectionMatrixUniform = uniformLocation("projectionMatrix");
    //_normalMatrixUniform = uniformLocation("normalMatrix");
}
namespace internal {
template <typename Drawable>
void pfgui(Drawable& d, const std::string& name_) {
    std::string name = name_;
    if (name.empty()) {
        name = "Polynomial Field Shader";
    }
    if (ImGui::TreeNode(name.c_str())) {
        bool vis = d.is_visible();
        ImGui::Checkbox("Visible", &vis);
        d.set_visibility(vis);
        ImGui::TreePop();
    }
}
template <typename Coefficients>
bool pfcgui(Coefficients& coefficients, const std::string& name_,
            int index = 0) {
    std::string name = name_;
    if (name.empty()) {
        name = fmt::format("Polynomial Field Shader Coefficients {}", index);
    }
    bool ret = false;
    if (ImGui::TreeNode(name.c_str())) {
        if (ImGui::InputInt("Degree", &coefficients.degree)) {
            ret = true;
            // poly_shader.setDegree(coefficients.degree);
        }
        if (ImGui::InputFloat2("Center", coefficients.center.data())) {
            ret = true;
            // poly_shader.setQuadratic(coefficients.quadratic);
        }
        if (ImGui::InputFloat("Scale", &coefficients.scale)) {
            ret = true;
            // poly_shader.setQuadratic(coefficients.quadratic);
        }
        if (ImGui::InputFloat("Constant", &coefficients.constant)) {
            ret = true;
            // poly_shader.setConstant(coefficients.constant);
        }
        if (ImGui::InputFloat2("Linear", coefficients.linear.data())) {
            ret = true;
            // poly_shader.setLinear(coefficients.linear);
        }
        if (ImGui::InputFloat4("Quadratic", coefficients.quadratic.data())) {
            ret = true;
            // poly_shader.setQuadratic(coefficients.quadratic);
        }
        // poly_shader.setPolynomialCoefficients(coefficients);
        ImGui::TreePop();
    }
    return ret;
}
}  // namespace internal
template <>
void DrawableBase<PolynomialScalarFieldShader<2>>::gui(
    const std::string& name_) {
    internal::pfgui(*this, name_);
}
template <>
void DrawableBase<PolynomialScalarFieldShader<3>>::gui(
    const std::string& name_) {
    internal::pfgui(*this, name_);
}
template <>
bool PolynomialScalarFieldShader<2>::PolynomialCoefficients::gui(
    const std::string& name_) {
    return internal::pfcgui(*this, name_);
}
template <>
bool PolynomialScalarFieldShader<3>::PolynomialCoefficients::gui(
    const std::string& name_) {
    return internal::pfcgui(*this, name_);
}
template <>
void DrawableMesh<PolynomialScalarFieldShader<2>>::set_buffers() {
    _mesh.addVertexBuffer(_mesh.vertex_buffer, 0,
                          PolynomialScalarFieldShader<2>::Position{});
}
template <>
void DrawableMesh<PolynomialScalarFieldShader<3>>::set_buffers() {
    _mesh.addVertexBuffer(_mesh.vertex_buffer, 0,
                          PolynomialScalarFieldShader<3>::Position{});
}
}  // namespace mtao::opengl
