#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include "mtao/opengl/drawables.h"

namespace mtao::opengl {
template<int D>
class VectorFieldShader : public Magnum::GL::AbstractShaderProgram {
  public:
    using Generic = std::conditional_t<D == 2, Magnum::Shaders::Generic2D, Magnum::Shaders::Generic3D>;
    using MatType =
      std::conditional_t<D == 2, Magnum::Matrix3, Magnum::Matrix4>;
    using Position = typename Generic::Position;

    using Vector = Magnum::GL::Attribute<5, Magnum::Math::Vector<D, float>>;
    using Color3 = typename Generic::Color3;
    using Color4 = typename Generic::Color4;
    enum : Magnum::UnsignedInt { PerVectorColors = 0,
                                 UniformColor = 1 };
    explicit VectorFieldShader(Magnum::UnsignedInt colorMode = UniformColor);

    VectorFieldShader &setColor(const Magnum::Color4 &color) {
        if (_colorMode == UniformColor) {
            setUniform(_colorUniform, color);
        }
        return *this;
    }
    VectorFieldShader &setTransformationProjectionMatrix(
      const MatType &matrix) {
        setUniform(_transformationProjectionMatrixUniform, matrix);
        return *this;
    }
    VectorFieldShader &setScale(float scale) {
        setUniform(_scaleUniform, scale);
        return *this;
    }
    /*
    VectorFieldShader& setTransformationMatrix(const MatType& matrix) {
        setUniform(_transformationMatrixUniform, matrix);
        return *this;
    }
    */

    VectorFieldShader &setNormalMatrix(const Magnum::Matrix3x3 &matrix) {
        setUniform(_normalMatrixUniform, matrix);
        return *this;
    }

    /*
    VectorFieldShader& setProjectionMatrix(const MatType& matrix) {
        setUniform(_projectionMatrixUniform, matrix);
        return *this;
    }
    */
    Magnum::UnsignedInt colorMode() const { return _colorMode; }

  private:
    void initialize();
    Magnum::Int _colorUniform, _transformationProjectionMatrixUniform,
      _scaleUniform,
      //_transformationMatrixUniform,
      //_projectionMatrixUniform,
      _normalMatrixUniform;
    const Magnum::UnsignedInt _colorMode;
};
template<int D>
struct ShaderData<VectorFieldShader<D>> {
    Color4 color = 0xff0033_rgbf;
    float scale = 1.0;
};
template<>
VectorFieldShader<2>::VectorFieldShader(Magnum::UnsignedInt colorMode);
template<>
VectorFieldShader<3>::VectorFieldShader(Magnum::UnsignedInt colorMode);
template<>
void DrawableBase<VectorFieldShader<2>>::gui(const std::string &name_);
template<>
void MeshDrawable<VectorFieldShader<2>>::gui(const std::string &name_);
template<>
void MeshDrawable<VectorFieldShader<2>>::set_buffers();
template<>
void DrawableBase<VectorFieldShader<3>>::gui(const std::string &name_);
template<>
void MeshDrawable<VectorFieldShader<3>>::set_buffers();

}// namespace mtao::opengl
