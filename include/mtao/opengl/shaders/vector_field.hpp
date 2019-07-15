#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include "mtao/opengl/shaders/drawables.hpp"



namespace mtao::opengl {
    class VectorFieldShader: public Magnum::GL::AbstractShaderProgram {
        public:
enum: Magnum::UnsignedInt {
          PerVectorColors = 0,
          UniformColor = 1
      };
      explicit VectorFieldShader(Magnum::UnsignedInt colorMode = UniformColor);

      VectorFieldShader& setColor(const Magnum::Color4& color) {
          setUniform(_colorUniform, color);
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
        private:
      Magnum::Int _colorUniform,
          _transformationMatrixUniform,
          _normalMatrixUniform,
          _projectionMatrixUniform;
      const Magnum::UnsignedInt _colorMode;
    };
    template <>
        void Drawable<VectorFieldShader>::gui(const std::string& name_);
}

