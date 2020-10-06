#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Arguments.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColor.h>
#include <mtao/types.h>

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>

#include "imgui.h"
#include "mtao/geometry/bounding_box.hpp"
#include "mtao/geometry/mesh/boundary_facets.h"
#include "mtao/geometry/mesh/read_obj.hpp"
#include "mtao/opengl/Window.h"
#include "mtao/opengl/drawables.h"
#include "mtao/opengl/objects/mesh.h"
#include "mtao/opengl/shaders/polynomial_scalar_field.hpp"
#include "mtao/opengl/objects/partitioned_mesh.hpp"
#include "mtao/opengl/objects/partitioned_polynomial_shaded_mesh.hpp"
bool animate = false;
bool save_frame = false;
ImVec4 clear_color = ImColor(114, 144, 154);

void gui_func() {
    {
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::Checkbox("Animate", &animate);
        ImGui::Checkbox("Store", &save_frame);

        // renderer->imgui_interface();
    }
    if (ImGui::Button("Reset  Camera?")) {
        // cam.reset();
    }
}

class MeshViewer : public mtao::opengl::Window2 {
   public:
    mtao::opengl::PolynomialScalarFieldShader<2>::PolynomialCoefficients
        coefficients;
    float colormap_scale = 1;
    float colormap_shift = 0;
    float min_value = -1;
    float max_value = 1;

    MeshViewer(const Arguments& args) : Window2(args), pmesh(&drawables()) {
        mtao::ColVecs2f V(2, 6);
        mtao::ColVecs3i F(3, 4);

        V.col(0) << -1, -1;
        V.col(1) << 1, -1;
        V.col(2) << 1, 1;
        V.col(3) << -1, 1;
        V.col(4) << 1, 2;
        V.col(5) << -1, 2;

        F.col(0) << 0, 1, 2;
        F.col(1) << 0, 2, 3;

        F.col(2) << 2, 5, 3;
        F.col(3) << 5, 2, 4;

        std::vector<int> partitions = {0, 2, 3, 4};

        auto bb = mtao::geometry::bounding_box(V);
        mtao::Vec2f mean = (bb.min() + bb.max()) / 2;
        V.colwise() -= mean;
        V.row(0).array() -= 1;
        pmesh.setTriangleBuffer(V.topRows<2>(), F.cast<unsigned int>());
        V.row(0).array() += 2;
        mesh.setTriangleBuffer(V.topRows<2>(), F.cast<unsigned int>());
        auto E = mtao::geometry::mesh::boundary_facets(F);

        mesh.setEdgeBuffer(E.cast<unsigned int>());
        pmesh.setEdgeBuffer(E.cast<unsigned int>());

        drawable = new mtao::opengl::Drawable<
            mtao::opengl::PolynomialScalarFieldShader<2>>{mesh, poly_shader,
                                                          drawables()};
        drawable->activate_triangles();
        drawable->activate_edges();
        poly_shader.setPolynomialCoefficients(coefficients);
        mesh.setParent(&root());
        pmesh.setParent(&root());
        pmesh.setCount(pmesh.triangle_Count);
        pmesh.setIndexBuffer(pmesh.triangle_index_buffer, 0,
                            pmesh.triangle_indexType, pmesh.triangle_indexStart,
                            pmesh.triangle_indexEnd);
        pmesh.setPrimitive(Magnum::GL::MeshPrimitive::Triangles);

        pmesh.set_offsets(partitions);
    }
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
    void gui() override {
        gui_func();
        if (drawable) {
            drawable->gui();
            if (coefficients.gui()) {
                poly_shader.setPolynomialCoefficients(coefficients);
            }
            if (ImGui::InputFloat("Colormap Scale", &colormap_scale)) {
                poly_shader.setColormapScale(colormap_scale);
                update_minmax();
            }
            if (ImGui::InputFloat("Colormap Shift", &colormap_shift)) {
                poly_shader.setColormapShift(colormap_shift);
                update_minmax();
            }
            if (ImGui::InputFloat("Colormap min", &min_value)) {
                update_scale_shift();
                poly_shader.setColormapScale(colormap_scale);
                poly_shader.setColormapShift(colormap_shift);
            }
            if (ImGui::InputFloat("Colormap max", &max_value)) {
                update_scale_shift();
                poly_shader.setColormapScale(colormap_scale);
                poly_shader.setColormapShift(colormap_shift);
            }
        }
    }
    void draw() override { Window2::draw(); }

   private:
    Magnum::Shaders::Flat2D flat_shader;
    mtao::opengl::PolynomialScalarFieldShader<2> poly_shader;
    mtao::opengl::objects::Mesh<2> mesh;
    Magnum::GL:::MeshView mesh_view;
    mtao::opengl::objects::PartitionedPolynomialShadedMesh<2> pmesh;
    // mtao::opengl::Drawable<Magnum::Shaders::Flat2D>* drawable = nullptr;
    mtao::opengl::Drawable<mtao::opengl::PolynomialScalarFieldShader<2>>*
        drawable = nullptr;
};

MAGNUM_APPLICATION_MAIN(MeshViewer)
