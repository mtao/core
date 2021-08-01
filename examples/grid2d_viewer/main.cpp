#include <iostream>

#include "mtao/opengl/Window.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#include <Eigen/Geometry>
#pragma GCC diagnostic pop
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayView.h>
#include <misc/cpp/imgui_stdlib.h>
#include <Corrade/Utility/Arguments.h>
#include <Magnum/EigenIntegration/Integration.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Shaders/VertexColor.h>
#include <mtao/eigen/stack.h>
#include <mtao/eigen_utils.h>
#include <mtao/geometry/grid/grid.h>
#include <mtao/opengl/objects/grid.h>
#include <mtao/types.h>
#include <mtao/visualization/imgui/colormap_settings_widget.hpp>

#include <Eigen/IterativeLinearSolvers>
#include <algorithm>
#include <memory>
#include <mtao/opengl/shaders/vector_field.hpp>

#include "imgui.h"
#include "mtao/geometry/bounding_box.hpp"
#include <pybind11/eigen.h>
#include "mtao/geometry/mesh/boundary_facets.h"
#include "mtao/geometry/mesh/read_obj.hpp"
#include "mtao/opengl/drawables.h"


#if defined(ENABLE_PYTHON_GUI)
#include <mtao/python/load_python_function.hpp>
#endif

using namespace Magnum::Math::Literals;


using namespace mtao::opengl;

class MeshViewer : public mtao::opengl::Window2 {
  public:
    enum class Mode : int { Smoothing,
                            LSReinitialization };
    Mode mode = Mode::LSReinitialization;

    float permeability = 100.0;
    float timestep = 1000.0;
    bool animate = false;
    using Vec = mtao::VectorX<GLfloat>;
    using Vec2 = mtao::Vec2f;
    Vec data;
    Vec data_original;
    Vec dx;
    Vec dy;
    Vec signs;
    Eigen::AlignedBox<float, 2> bbox;
    Eigen::SparseMatrix<float> L;

    std::array<int, 2> N{ { 20, 20 } };
    int &NI = N[0];
    int &NJ = N[1];
    Vector2 cursor;
    mtao::vector<mtao::Vec2f> points;

    Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;

    MeshViewer(const Arguments &args) : Window2(args) {
        bbox.min().setConstant(-1);
        bbox.max().setConstant(1);

        edge_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{
            grid, _flat_shader, drawables()
        };
        edge_drawable->activate_triangles({});
        edge_drawable->activate_edges();
        face_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D>{ grid, _vcolor_shader, drawables() };
        grid.setParent(&root());
        cursor_mesh.setParent(&scene());
        cursor_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{
            cursor_mesh, _flat_shader, drawables()
        };
        cursor_mesh.setVertexBuffer(mtao::Vec2f::Zero().eval());
        cursor_drawable->data().color = 0xffffff_rgbf;
        visible_grid.setParent(&scene());
        visible_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{
            visible_grid, _flat_shader, drawables()
        };
        vfield_mesh.setParent(&scene());
        _vf_viewer =
          new mtao::opengl::MeshDrawable<mtao::opengl::VectorFieldShader<2>>{
              vfield_mesh, _vf_shader, drawables()
          };
        update();
    }
    void update() {
        // mtao::geometry::grid::Grid2f g(std::array<int,2>{{NI,NJ,NK}});
        auto g = mtao::geometry::grid::Grid2f::from_bbox(
          bbox, std::array<int, 2>{ { NI, NJ } });

        auto V = g.vertices();
        mtao::VecXf vals = V.colwise().norm().transpose();

        grid.setColorBuffer(colmap_widget.get_rgba(vals));
        // mtao::RowVectorX<float> C = V.colwise()
        //grid.setColorBuffer(
        //  mtao::eigen::vstack(g.vertices().array().pow(.5)
        //                      //,mtao::RowVectorX<float>::Zero(g.size())
        //                      ,
        //                      1 - g.vertices().colwise().sum().array() / 2,
        //                      mtao::RowVectorX<float>::Ones(g.size())));
        grid.set(g);
        {
            vfield_mesh.setVertexBuffer(g.vertices());
            mtao::ColVecs2f V = g.vertices();
            // V.array() -= .5;

            if (func) {
                try {
                    for (int j = 0; j < V.cols(); ++j) {
                        V.col(j) = (*func)(V.col(j)).cast<Eigen::Vector2f>();
                    }
                } catch (std::exception &e) {
                    spdlog::warn(e.what());
                    V.setZero();
                }
            } else {
                auto x = V.row(0).eval();
                V.row(0) = -V.row(1);
                V.row(1) = x;
            }
            vfield_mesh.setVFieldBuffer(V);
        }

        cursor_drawable->deactivate();
        cursor_drawable->activate_points();
        _vf_viewer->deactivate();
        _vf_viewer->activate_points();
        {
            auto g = mtao::geometry::grid::Grid2f(std::array<int, 2>{ { 2, 2 } });

            // auto V = g.vertices();
            // mtao::RowVectorX<float> C = V.colwise()
            visible_grid.set(g);
            visible_drawable->deactivate();
            visible_drawable->activate_edges();
        }
    }
    void do_animation() {}
    float scale = 1.0;

#if defined(ENABLE_PYTHON_GUI)
    void update_function(const std::string &str) {
        if (func) {
            func->update_function(str);
        } else {
            func.emplace(str);
        }
        update();
    }
#endif
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
        if (ImGui::TreeNode("Colormap")) {
            if (colmap_widget.gui()) {
                update();
            }
            ImGui::TreePop();
        }


#if defined(ENABLE_PYTHON_GUI)
        ImGui::Begin("PythonGui");
        //ImGui::Text

        static std::string text = "def FUNC_NAME(x): return x";
        ImGui::InputTextMultiline("Code", &text);
        if (ImGui::Button("Activate")) {
            update_function(text);
        }
        ImGui::End();
#endif

        ImGui::Text("Cursor Position: (%f,%f)", cursor.x(), cursor.y());
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
                points.emplace_back(
                  EigenIntegration::cast<mtao::Vec2f>(cursor));
                cursor_mesh.setVertexBuffer(mtao::eigen::stl2eigen(points));
                if (points.size() > 1) {
                    mtao::ColVecs2i E(2, points.size());
                    E.row(0) = mtao::RowVecXi::LinSpaced(points.size(), 0, points.size() - 1);
                    E.row(1).rightCols(E.cols() - 1) =
                      E.row(0).leftCols(E.cols() - 1);
                    E(1, 0) = points.size() - 1;
                    cursor_mesh.setEdgeBuffer(E.cast<unsigned int>());
                    cursor_drawable->activate_edges();
                }
            }
        }
    }

  private:
#if defined(ENABLE_PYTHON_GUI)
    std::optional<mtao::python::PythonFunction> func;
#endif
    Magnum::Shaders::Flat2D _flat_shader;
    Magnum::Shaders::VertexColor2D _vcolor_shader;
    mtao::opengl::VectorFieldShader<2> _vf_shader;
    mtao::opengl::objects::Grid<2> grid;
    mtao::opengl::objects::Mesh<2> cursor_mesh;
    mtao::opengl::objects::Grid<2> visible_grid;
    mtao::opengl::objects::Mesh<2> vfield_mesh;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D> *edge_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D> *face_drawable =
      nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D> *cursor_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D> *visible_drawable = nullptr;
    mtao::opengl::MeshDrawable<mtao::opengl::VectorFieldShader<2>> *_vf_viewer =
      nullptr;

    mtao::visualization::imgui::ColorMapSettingsWidget colmap_widget;
};

MAGNUM_APPLICATION_MAIN(MeshViewer)
