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
#include "mtao/opengl/objects/partitioned_mesh.hpp"
#include "mtao/opengl/objects/partitioned_polynomial_shaded_mesh.hpp"
#include "mtao/opengl/shaders/polynomial_scalar_field.hpp"
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
    MeshViewer(const Arguments& args)
        : Window2(args), pmesh(&drawables()), mesh_view(pmesh) {
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
        V.row(0).array() += 2.01;
        mesh.setTriangleBuffer(V.topRows<2>(), F.cast<unsigned int>());
        auto E = mtao::geometry::mesh::boundary_facets(F);

        mesh.setEdgeBuffer(E.cast<unsigned int>());
        pmesh.setEdgeBuffer(E.cast<unsigned int>());

        drawable = new mtao::opengl::MeshDrawable<
            mtao::opengl::PolynomialScalarFieldShader<2>>{mesh, poly_shader,
                                                          drawables()};
        drawable->activate_triangles();
        drawable->activate_edges();
        // poly_shader.setPolynomialCoefficients(coefficients);
        mesh.setParent(&root());
        pmesh.setParent(&root());
        pmesh.setCount(pmesh.triangle_Count);
        pmesh.setIndexBuffer(
            pmesh.triangle_index_buffer, 0, pmesh.triangle_indexType,
            pmesh.triangle_indexStart, pmesh.triangle_indexEnd);
        pmesh.setPrimitive(Magnum::GL::MeshPrimitive::Triangles);
        drawable->data().coefficients.emplace();

        pmesh.set_offsets(partitions);

        // vdrawable = new mtao::opengl::ViewDrawable<
        //    mtao::opengl::PolynomialScalarFieldShader<2>>{
        //    mesh_view, poly_shader, drawables()};
    }
    void gui() override {
        gui_func();
        if (drawable) {
            drawable->gui();
        }
        pmesh.gui();
        // if (pdrawable) {
        //    pdrawable->gui("second");
        //}
    }
    void draw() override { Window2::draw(); }

   private:
    Magnum::Shaders::Flat2D flat_shader;
    mtao::opengl::PolynomialScalarFieldShader<2> poly_shader;
    mtao::opengl::objects::Mesh<2> mesh;
    mtao::opengl::objects::PartitionedPolynomialShadedMesh<2> pmesh;
    Magnum::GL::MeshView mesh_view;
    // mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>* drawable = nullptr;
    mtao::opengl::MeshDrawable<mtao::opengl::PolynomialScalarFieldShader<2>>*
        drawable = nullptr;
    // mtao::opengl::MeshDrawable<mtao::opengl::PolynomialScalarFieldShader<2>>*
    //    pdrawable = nullptr;
};

MAGNUM_APPLICATION_MAIN(MeshViewer)
