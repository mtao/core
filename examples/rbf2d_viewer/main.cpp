#include <iostream>

#include "mtao/opengl/Window.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#include <Eigen/Geometry>
#pragma GCC diagnostic pop
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Arguments.h>
#include <Magnum/EigenIntegration/Integration.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Shaders/VertexColor.h>
#include <colormap/colormap.h>
#include <mtao/eigen/stack.h>
#include <mtao/eigen_utils.h>
#include <mtao/geometry/grid/grid.h>
#include <mtao/opengl/objects/grid.h>

#include <algorithm>
#include <memory>
#include <mtao/geometry/interpolation/radial_basis_function.hpp>
#include <mtao/opengl/shaders/vector_field.hpp>
#include <mtao/types.hpp>

#include "imgui.h"
#include "mtao/geometry/bounding_box.hpp"
#include "mtao/geometry/mesh/boundary_facets.h"
#include "mtao/geometry/mesh/read_obj.hpp"
#include "mtao/opengl/drawables.h"

using namespace Magnum::Math::Literals;

using namespace mtao::opengl;

class MeshViewer : public mtao::opengl::Window2 {
  public:
    float timestep = 1000.0;
    bool animate = false;
    using Vec = mtao::VectorX<GLfloat>;
    using Vec2 = mtao::Vec2f;
    Eigen::AlignedBox<float, 2> bbox;
    Eigen::SparseMatrix<float> L;

    std::array<int, 2> N{ { 20, 20 } };
    int &NI = N[0];
    int &NJ = N[1];
    Vector2 cursor;
    mtao::Vec2f center = mtao::Vec2f(0.f, 0.f);
    float radius = 0.3f;

    int mode = 0;

    MeshViewer(const Arguments &args) : Window2(args) {
        bbox.min().setConstant(-1);
        bbox.max().setConstant(1);

        edge_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{
            grid, _flat_shader, drawables()
        };
        edge_drawable->activate_triangles({});
        edge_drawable->activate_edges();
        grid.setParent(&root());

        face_drawable =
          new mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D>{
              grid, _vcolor_shader, drawables()
          };

        visible_grid.setParent(&scene());
        visible_drawable =
          new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{
              visible_grid, _flat_shader, drawables()
          };
        vfield_mesh.setParent(&scene());
        //_vf_viewer =
        //    new
        //    mtao::opengl::MeshDrawable<mtao::opengl::VectorFieldShader<2>>{
        //        vfield_mesh, _vf_shader, drawables()};
        update();
    }
    auto rbf(auto &&p) const {
        using namespace mtao::geometry::interpolation;

        switch (mode) {
        case 0:
            return spline_gaussian_rbf(p, center, radius);
        case 1:
        default:
            return desbrun_spline_rbf(p, center, radius);
        }
    }
    auto rbf_grad(auto &&p) const {
        using namespace mtao::geometry::interpolation;

        switch (mode) {
        case 0:
            return spline_gaussian_rbf_gradient(p, center, radius);
        case 1:
        default:
            return desbrun_spline_rbf_gradient(p, center, radius);
        }
    }
    float rbf() const { return rbf(Vec2(cursor.x(), cursor.y()))(0); }
    void update() {
        auto g = mtao::geometry::grid::Grid2f::from_bbox(
          bbox, std::array<int, 2>{ { NI, NJ } });
        grid.set(g);
        {
            vfield_mesh.setVertexBuffer(g.vertices());
            mtao::ColVecs2f V = g.vertices();
            // V.array() -= .5;
            auto x = V.row(0).eval();
            V.row(0) = -V.row(1);
            V.row(1) = x;
            vfield_mesh.setVFieldBuffer(V);
        }
        update_rbf();

        //_vf_viewer->deactivate();
        //_vf_viewer->activate_points();
        {
            auto g = mtao::geometry::grid::Grid2f(std::array<int, 2>{ { 2, 2 } });

            // auto V = g.vertices();
            // mtao::RowVectorX<float> C = V.colwise()
            visible_grid.set(g);
            visible_drawable->deactivate();
            visible_drawable->activate_edges();
        }
    }
    std::function<mtao::Vec4f(float)> get_colormap_func() const {
        return [](float value) -> mtao::Vec4f {
            colormap::MATLAB::Jet cm;
            auto c = cm.getColor(value);
            mtao::Vec4f r;
            r.x() = c.r;
            r.y() = c.g;
            r.z() = c.b;
            r.w() = 1.0;
            return r;
        };
    }
    void update_rbf() {
        // mtao::geometry::grid::Grid2f
        // g(std::array<int,2>{{NI,NJ,NK}});
        auto g = mtao::geometry::grid::Grid2f::from_bbox(
          bbox, std::array<int, 2>{ { NI, NJ } });

        auto colormap_func = get_colormap_func();

        // auto V = g.vertices();
        auto V = g.vertices();

        double sum = 0;
        mtao::ColVecs4f C(4, V.cols());
        for (int j = 0; j < V.cols(); ++j) {
            auto v = V.col(j);
            float val = rbf(v)(0);
            sum += val;
            auto c = C.col(j);
            c = colormap_func(val);
        }
        sum /= V.cols();
        sum *= bbox.volume();
        std::cout << "Integrated volume: " << sum << std::endl;
        grid.setColorBuffer(C);
    }
    void do_animation() {}
    float scale = 1.0;
    void gui() override {
        if (ImGui::InputInt2("N", &NI)) {
            update();
        }
        if (ImGui::SliderFloat2("min", bbox.min().data(), -2, 2)) {
            bbox.min() = (bbox.min().array() < bbox.max().array())
                           .select(bbox.min(), bbox.max());
            update();
        }
        if (ImGui::SliderFloat2("max", bbox.max().data(), -2, 2)) {
            bbox.max() = (bbox.min().array() > bbox.max().array())
                           .select(bbox.min(), bbox.max());
            update();
        }
        if (ImGui::SliderFloat("Radius", &radius, -2, 2)) {
            update();
        }
        if (_vf_viewer) {
            _vf_viewer->gui();
        }
        if (edge_drawable) {
            edge_drawable->gui();
        }
        if (face_drawable) {
            face_drawable->gui();
        }
        if (ImGui::Button("Step")) {
            do_animation();
        }
        ImGui::Text("Cursor Position: (%f,%f) = %f (d=%f)", cursor.x(), cursor.y(), rbf(), (mtao::Vec2f(cursor.x(), cursor.y()) - center).norm());
    }
    void draw() override {
        if (animate) {
            do_animation();
        }
        Magnum::GL::Renderer::disable(
          Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::setPointSize(10.);
        Window2::draw();
    }

    void mouseMoveEvent(MouseMoveEvent &event) override {
        Window2::mouseMoveEvent(event);
        cursor = localPosition(event.position());
    }
    void mousePressEvent(MouseEvent &event) override {
        Window2::mousePressEvent(event);
        if (!ImGui::GetIO().WantCaptureMouse) {
            if (event.button() == MouseEvent::Button::Left) {
                center = EigenIntegration::cast<mtao::Vec2f>(cursor);
                update_rbf();
            }
        }
    }

  private:
    Magnum::Shaders::Flat2D _flat_shader;
    Magnum::Shaders::VertexColor2D _vcolor_shader;
    mtao::opengl::VectorFieldShader<2> _vf_shader;
    mtao::opengl::objects::Grid<2> grid;
    mtao::opengl::objects::Grid<2> visible_grid;
    mtao::opengl::objects::Mesh<2> vfield_mesh;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D> *edge_drawable =
      nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D> *face_drawable =
      nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D> *visible_drawable =
      nullptr;
    mtao::opengl::MeshDrawable<mtao::opengl::VectorFieldShader<2>> *_vf_viewer =
      nullptr;
};

MAGNUM_APPLICATION_MAIN(MeshViewer)
