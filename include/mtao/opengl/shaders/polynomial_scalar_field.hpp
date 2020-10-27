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
#include <optional>
#include <span>

#include "mtao/opengl/drawables.h"
#include "mtao/types.hpp"

namespace mtao::opengl {

template <int D>
class PolynomialScalarFieldShader;
template <int D>
struct ShaderData<PolynomialScalarFieldShader<D>> {
    struct PolynomialCoefficients {
        int degree = 2;
        float constant = 0;
        Magnum::Math::Vector<D, float> linear;
        Magnum::Math::Matrix<D, float> quadratic;
        // weirdly this is indexed k,i,j
        Corrade::Containers::Array<Magnum::Math::Matrix<D, float>> cubic;
        float scale = 1;
        Magnum::Math::Vector<D, float> center;
        bool gui(const std::string& name = "");

        void zero();
    };
    // for debug purposes one should change min_value/max_value with
    // update_minmax() whenever colormap_scale / colormap_shift are changed
    float colormap_scale = 1;
    float colormap_shift = 0;

    // convenience representation, should always call update_scale_shift() any
    // time these values are modified to activate the changes
    float min_value = -1;
    float max_value = 1;

    void update_minmax() {
        // [-1,1] = scale * [min,max] + shift
        // [-1-shift,1-shift] = scale * [min,max]
        //
        min_value = (-1 - colormap_shift) / colormap_scale;
        max_value = (1 - colormap_shift) / colormap_scale;
    }
    void update_scale_shift() {
        // [-1,1] = scale * [min,max] + shift
        // [-1-shift,1-shift] = scale * [min,max]
        // -(1+scale)/shift = min
        // (1-scale)/shift = max

        colormap_scale = 2 / (max_value - min_value);
        colormap_shift = (min_value + max_value) / (min_value - max_value);
    }

    // in the case that this shader is being shared by multiple objects we may
    // want to change these coefficients up in our default draw mode
    // set_buffers() will always update teh colormap values, but will only on
    // occasion set the data value
    std::optional<PolynomialCoefficients> coefficients;
};
template <int D>
class PolynomialScalarFieldShader : public Magnum::GL::AbstractShaderProgram {
   public:
    using Generic = std::conditional_t<D == 2, Magnum::Shaders::Generic2D,
                                       Magnum::Shaders::Generic3D>;
    using MatType =
        std::conditional_t<D == 2, Magnum::Matrix3, Magnum::Matrix4>;
    using Position = typename Generic::Position;

    using PolynomialCoefficients = typename ShaderData<
        PolynomialScalarFieldShader<D>>::PolynomialCoefficients;
    enum : Magnum::UnsignedInt { Parula = 0, Jet = 1, Waves = 2 };
    explicit PolynomialScalarFieldShader(Magnum::UnsignedInt colorMode = Jet);

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

    Magnum::UnsignedInt colorMode() const { return _colorMode; }

   private:
    void initialize();
    Magnum::Int _transformationProjectionMatrixUniform{0}, _degree, _constant,
        _linear, _quadratic, _cubic, _scale, _center, _colormap_scale,
        _colormap_shift

        ;
    const Magnum::UnsignedInt _colorMode;
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
void MeshDrawable<PolynomialScalarFieldShader<2>>::set_buffers();
template <>
void DrawableBase<PolynomialScalarFieldShader<3>>::gui(
    const std::string& name_);
template <>
void MeshDrawable<PolynomialScalarFieldShader<3>>::set_buffers();

template <>
bool PolynomialScalarFieldShader<2>::PolynomialCoefficients::gui(
    const std::string& name_);
template <>
bool PolynomialScalarFieldShader<3>::PolynomialCoefficients::gui(
    const std::string& name_);

template <>
void PolynomialScalarFieldShader<2>::PolynomialCoefficients::zero();
template <>
void PolynomialScalarFieldShader<3>::PolynomialCoefficients::zero();

}  // namespace mtao::opengl

