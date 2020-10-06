#pragma once
#include <Magnum/GL/MeshView.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <Eigen/Geometry>

#include "imgui.h"
#include "mtao/iterator/enumerate.hpp"
#include "mtao/iterator/zip.hpp"
#include "mtao/opengl/drawables.h"
#include "mtao/opengl/objects/partitioned_mesh.hpp"
#include "mtao/opengl/shaders/polynomial_scalar_field.hpp"

namespace mtao::opengl::objects {
// a confusing object that is a combination of drawable
using namespace Magnum;
template <int D>
class PartitionedPolynomialShadedMesh
    : public PartitionedMesh<D>,
      std::conditional_t<D == 2, SceneGraph::Drawable2D,
                         SceneGraph::Drawable3D> {
   public:
    using MeshType = Mesh<D>;
    using ShaderType = PolynomialScalarFieldShader<D>;
    using TransMat = internal::TransformMatrixType<ShaderType>;
    using DrawableType = internal::DrawableType<ShaderType>;
    using DrawableGroup = internal::DrawableGroupType<ShaderType>;
    using Camera = internal::CameraType<ShaderType>;
    PartitionedPolynomialShadedMesh(DrawableGroup* group = nullptr)
        : DrawableType(*this, group) {}

   private:
    float colormap_scale;
    float colormap_shift;
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

   public:
    void draw(const TransMat& transformationMatrix, Camera& camera) override;
    void set_offsets(const std::vector<int>& offsets);

    std::vector<
        typename PolynomialScalarFieldShader<D>::PolynomialCoefficients>&
    coefficients();

    void gui();

   private:
    Corrade::Containers::Array<Magnum::GL::MeshView> _views;
    ShaderType _shader;
    DrawableGroup* _drawable_group;
    std::vector<typename PolynomialScalarFieldShader<D>::PolynomialCoefficients>
        _coefficients;
};
template <int D>
void PartitionedPolynomialShadedMesh<D>::draw(
    const TransMat& transformationMatrix, Camera& camera) {
    // MeshType::setIndexBuffer(MeshType::triangle_index_buffer, 0,
    //                         MeshType::triangle_indexType, 0, 0);
    return;
    MeshType::setPrimitive(GL::MeshPrimitive::Triangles);

    _shader.setTransformationProjectionMatrix(camera.projectionMatrix() *
                                              transformationMatrix);

    for (auto&& [coeffs, view] : mtao::iterator::zip(_coefficients, _views)) {
        _shader.setPolynomialCoefficients(coeffs);
        _shader.draw(view);
    }
    //_shader.draw(*this);
}
template <int D>
void PartitionedPolynomialShadedMesh<D>::set_offsets(
    const std::vector<int>& offsets) {
    PartitionedMesh<D>::set_offsets(offsets);
    _views = PartitionedMesh<D>::views();
    _coefficients.resize(offsets.size() - 1);
    // for (auto&& view : _views) {
    //    new DrawableType{view, _drawable_group};
    //}
}
// template <int D>
// void PartitionedPolynomialShadedMesh<D>::set_coefficients(
//    const std::vector<
//        typename PolynomialScalarFieldShader<D>::PolynomialCoefficients>&
//        coeffs) {
//    _coefficients = coeffs;
//}

template <int D>
void PartitionedPolynomialShadedMesh<D>::gui() {
    if (ImGui::InputFloat("Colormap Scale", &colormap_scale)) {
        _shader.setColormapScale(colormap_scale);
        update_minmax();
    }
    if (ImGui::InputFloat("Colormap Shift", &colormap_shift)) {
        _shader.setColormapShift(colormap_shift);
        update_minmax();
    }
    if (ImGui::InputFloat("Colormap min", &min_value)) {
        update_scale_shift();
        _shader.setColormapScale(colormap_scale);
        _shader.setColormapShift(colormap_shift);
    }
    if (ImGui::InputFloat("Colormap max", &max_value)) {
        update_scale_shift();
        _shader.setColormapScale(colormap_scale);
        _shader.setColormapShift(colormap_shift);
    }
    for (auto&& [index, coefficients] :
         mtao::iterator::enumerate(_coefficients)) {
        coefficients.gui(fmt::format("{}", index));
    }
}
}  // namespace mtao::opengl::objects
