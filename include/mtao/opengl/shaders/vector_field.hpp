#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include "mtao/opengl/drawables.h"



namespace mtao::opengl {
    template <int D>
    class VectorFieldShader: public Magnum::GL::AbstractShaderProgram {
        public:
            using Generic = std::conditional_t<D == 2,Magnum::Shaders::Generic2D,Magnum::Shaders::Generic2D>;
            using Position = typename Generic::Position;
            using Normal = std::conditional_t<D==2,void,Magnum::Shaders::Generic3D::Normal>;
            using Color3 = typename Generic::Color3;
            using Color4 = typename Generic::Color4;
enum: Magnum::UnsignedInt {
          PerVectorColors = 0,
          UniformColor = 1
      };
      explicit VectorFieldShader(Magnum::UnsignedInt colorMode = UniformColor);

      VectorFieldShader& setColor(const Magnum::Color4& color) {
          if(_colorMode == UniformColor) {
              setUniform(_colorUniform, color);
          }
          return *this;
      }
      VectorFieldShader& setTransformationMatrix(const Magnum::Matrix4& matrix) {
          setUniform(_transformationMatrixUniform, matrix);
          return *this;
      }

      VectorFieldShader& setNormalMatrix(const Magnum::Matrix3x3& matrix) {
          setUniform(_normalMatrixUniform, matrix);
          return *this;
      }

      VectorFieldShader& setProjectionMatrix(const Magnum::Matrix4& matrix) {
          setUniform(_projectionMatrixUniform, matrix);
          return *this;
      }
      Magnum::UnsignedInt colorMode() const { return _colorMode;}
        private:
      void initialize();
      Magnum::Int _colorUniform,
          _transformationMatrixUniform,
          _normalMatrixUniform,
          _projectionMatrixUniform;
      const Magnum::UnsignedInt _colorMode;
    };
    template <>
      VectorFieldShader<2>::VectorFieldShader(Magnum::UnsignedInt colorMode);
    template <>
      VectorFieldShader<3>::VectorFieldShader(Magnum::UnsignedInt colorMode);
    template <>
        void Drawable<VectorFieldShader<2>>::gui(const std::string& name_);
    template <>
        void Drawable<VectorFieldShader<2>>::set_buffers();
    template <>
        void Drawable<VectorFieldShader<3>>::gui(const std::string& name_);
    template <>
        void Drawable<VectorFieldShader<3>>::set_buffers();
}

