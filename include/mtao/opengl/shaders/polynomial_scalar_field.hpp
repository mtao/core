#pragma once
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayView.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix.h>
#include <Magnum/Math/Vector.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <colormap/colormap.h>

#include <iostream>
#include <span>

#include "mtao/opengl/drawables.h"

namespace mtao::opengl {
template <int D>
class PolynomialScalarFieldShader : public Magnum::GL::AbstractShaderProgram {
   public:
    struct PolynomialCoefficients {
        int degree = 2;
        float constant = 0;
        Magnum::Math::Vector<D, float> linear;
        Magnum::Math::Matrix<D, float> quadratic;
        Corrade::Containers::Array<Magnum::Math::Matrix<D, float>> cubic;
        float scale = 1;
        Magnum::Math::Vector<D, float> center;
        bool gui(const std::string& name = "");

        ;
    };
    using Generic = std::conditional_t<D == 2, Magnum::Shaders::Generic2D,
                                       Magnum::Shaders::Generic3D>;
    using MatType =
        std::conditional_t<D == 2, Magnum::Matrix3, Magnum::Matrix4>;
    using Position = typename Generic::Position;

    enum : Magnum::UnsignedInt { Parula = 0, Jet = 1, Waves = 2 };
    explicit PolynomialScalarFieldShader(
        Magnum::UnsignedInt colorMode = Parula);

    PolynomialScalarFieldShader& setTransformationProjectionMatrix(
        const MatType& matrix) {
        setUniform(_transformationProjectionMatrixUniform, matrix);
        return *this;
    }

    PolynomialScalarFieldShader& setPolynomialCoefficients(
        const PolynomialCoefficients& pd) {
        setDegree(pd.degree);
        setConstant(pd.constant);
        setLinear(pd.linear);
        setQuadratic(pd.quadratic);
        // setCubic(pd.cubic);
        setScale(pd.scale);
        setCenter(pd.center);
        return *this;
    }
    PolynomialScalarFieldShader& setDegree(int degree) {
        setUniform(_degree, degree);
        return *this;
    }
    PolynomialScalarFieldShader& setScale(float scale) {
        setUniform(_scale, scale);
        return *this;
    }
    PolynomialScalarFieldShader& setColormapScale(float scale) {
        setUniform(_colormap_scale, scale);
        return *this;
    }
    PolynomialScalarFieldShader& setColormapShift(float shift) {
        setUniform(_colormap_shift, shift);
        return *this;
    }
    PolynomialScalarFieldShader& setConstant(float value) {
        setUniform(_constant, value);
        return *this;
    }
    PolynomialScalarFieldShader& setLinear(
        const Magnum::Math::Vector<D, float>& value) {
        setUniform(_linear, value);
        return *this;
    }
    PolynomialScalarFieldShader& setCenter(
        const Magnum::Math::Vector<D, float>& value) {
        setUniform(_center, value);
        return *this;
    }
    PolynomialScalarFieldShader& setQuadratic(
        const Magnum::Math::Matrix<D, float>& value) {
        setUniform(_quadratic, value);
        return *this;
    }
    PolynomialScalarFieldShader& setCubic(
        const Corrade::Containers::ArrayView<
            const Magnum::Math::Matrix<D, float>>& value) {
        setUniform(_cubic, value);
        return *this;
    }

   private:
    void initialize();
    Magnum::Int _transformationProjectionMatrixUniform{0}, _degree, _constant,
        _linear, _quadratic, _cubic, _scale, _center, _colormap_scale,
        _colormap_shift

        ;
    const Magnum::UnsignedInt _colorMap;
};
template <>
PolynomialScalarFieldShader<2>::PolynomialScalarFieldShader(
    Magnum::UnsignedInt colorMode);
template <>
PolynomialScalarFieldShader<3>::PolynomialScalarFieldShader(
    Magnum::UnsignedInt colorMode);
template <>
void DrawableBase<PolynomialScalarFieldShader<2>>::gui(
    const std::string& name_);
template <>
void DrawableMesh<PolynomialScalarFieldShader<2>>::set_buffers();
template <>
void DrawableBase<PolynomialScalarFieldShader<3>>::gui(
    const std::string& name_);
template <>
void DrawableMesh<PolynomialScalarFieldShader<3>>::set_buffers();

template <>
bool PolynomialScalarFieldShader<2>::PolynomialCoefficients::gui(
    const std::string& name_);
template <>
bool PolynomialScalarFieldShader<3>::PolynomialCoefficients::gui(
    const std::string& name_);
}  // namespace mtao::opengl

