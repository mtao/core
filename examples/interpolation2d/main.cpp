#include "mtao/opengl/Window.h"
#include <iostream>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#include <Eigen/Geometry>
#pragma GCC diagnostic pop
#include "imgui.h"
#include <memory>
#include <algorithm>
#include "mtao/geometry/mesh/boundary_facets.h"
#include "mtao/geometry/mesh/read_obj.hpp"
#include "mtao/geometry/bounding_box.hpp"
#include "mtao/opengl/drawables.h"
#include <mtao/types.h>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayView.h>

#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/GL/Renderer.h>
#include <Corrade/Utility/Arguments.h>
#include <mtao/opengl/objects/grid.h>
#include <mtao/geometry/grid/grid.h>

#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <algorithm>

#include <mtao/eigen_utils.h>
#include <Eigen/IterativeLinearSolvers>
#include <Magnum/EigenIntegration/Integration.h>
#include "curve.hpp"
#include <mtao/geometry/grid/grid_data.hpp>
#include <mtao/geometry/winding_number.hpp>
#include <mtao/opengl/shaders/vector_field.hpp>
using namespace Magnum::Math::Literals;


using namespace mtao::opengl;


class MeshViewer : public mtao::opengl::Window2 {
  public:
    Curve::InterpMode interp_mode = Curve::InterpMode::MeanValue;
    using Vec = mtao::VectorX<GLfloat>;
    using Vec2 = mtao::Vec2f;
    Vec data;
    Vec data_original;
    Vec dx;
    Vec dy;
    Vec signs;
    Eigen::AlignedBox<float, 2> bbox;

    int active_weight_index = 0;


    std::array<int, 2> N{ { 300, 300 } };
    int &NI = N[0];
    int &NJ = N[1];
    Vector2 cursor;
    Curve curve;
    mtao::geometry::grid::Grid2f grid;
    mtao::geometry::grid::GridData2f grid_data;

    Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;

    MeshViewer(const Arguments &args) : Window2(args) {
        bbox.min().setConstant(-1);
        bbox.max().setConstant(1);

        curve.points.emplace_back(mtao::Vec2d(.8, .8));
        curve.points.emplace_back(mtao::Vec2d(.7, -.2));
        curve.points.emplace_back(mtao::Vec2d(-.8, -.2));
        curve.points.emplace_back(mtao::Vec2d(-.7, .8));

        edge_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{ grid_mesh, _flat_shader, drawables() };
        edge_drawable->activate_triangles({});
        edge_drawable->activate_edges();
        face_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D>{ grid_mesh, _vcolor_shader, gridcolor_drawgroup };
        grid_mesh.setParent(&root());
        curve_mesh.setParent(&root());
        curve_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{ curve_mesh, _flat_shader, drawables() };
        curve_drawable->deactivate();
        curve_mesh.setVertexBuffer(mtao::Vec2f::Zero().eval());
        curve_drawable->data().color = 0xff0000_rgbf;
        curve_drawable->activate_points();
        //
        //curve_point_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D>{curve_points,_vcolor_shader, drawables()};
        //curve_points.setVertexBuffer(mtao::Vec2f::Zero().eval());
        //curve_points.setParent(&root());
        //mtao::geometry::grid::Grid2f g(std::array<int,2>{{NI,NJ}});
        grid = mtao::geometry::grid::Grid2f::from_bbox(bbox, std::array<int, 2>{ { NI, NJ } });

        //grid_data.resize(std::array<int,2>{{NI,NJ}});

        //auto V = g.vertices();
        //mtao::RowVectorX<float> C = V.colwise()
        grid_mesh.setColorBuffer(
          mtao::eigen::vstack(
            grid.vertices().array().pow(.5)
            //,mtao::RowVectorX<float>::Zero(g.size())
            ,
            1 - grid.vertices().colwise().sum().array() / 2,
            mtao::RowVectorX<float>::Ones(grid.size())));
        grid_mesh.set(grid);
        //curve_point_drawable->deactivate();


        //vfield_mesh.setParent(&scene());
        //_vf_viewer = new mtao::opengl::MeshDrawable<mtao::opengl::VectorFieldShader<2>>{vfield_mesh,_vf_shader, drawables()};
        update();


        //_vf_viewer->deactivate();
        //_vf_viewer->activate_points();
    }
    void update() {
        auto &points = curve.points;
        grid = mtao::geometry::grid::Grid2f::from_bbox(bbox, std::array<int, 2>{ { NI, NJ } });
        grid_mesh.set(grid);
        active_weight_index = std::clamp<int>(active_weight_index, 0, points.size() - 1);

        //grid_data.resize(std::array<int,2>{{NI,NJ}});

        if (points.size() > 1) {
            curve_mesh.setEdgeBuffer(curve.V().cast<float>().eval(), curve.E().cast<unsigned int>());
            curve_drawable->activate_edges();
            curve_drawable->activate_points({});


            auto V = grid.vertices();
            auto evaluator = curve.evaluator(interp_mode);
            mtao::VecXd coeffs = mtao::VecXd::Unit(points.size(), active_weight_index);
            std::cout << "Coeffs: " << coeffs.transpose() << std::endl;
            //mtao::VecXf D = evaluator(Dat, V.cast<double>()).cast<float>();
            mtao::VecXf D = evaluator.from_coefficients(coeffs, V.cast<double>()).cast<float>();
            D = D.cwiseAbs();
            //
            mtao::ColVecs4f C(4, grid.size());
            {

                C.row(3).array() = 1;
                C.array() = 1;
                C.row(0) -= D.transpose();
                C.row(1) -= D.transpose();
                grid_mesh.setColorBuffer(C);
            }


            //{
            //    vfield_mesh.setVertexBuffer(V);
            //    vfield_mesh.setVFieldBuffer(G);
            //}

        } else {
            curve_mesh.setVertexBuffer(curve.V().cast<float>().eval());
        }
    }
    float scale = 1.0;
    void gui() override {
        if (ImGui::InputInt2("N", &NI)) {
            update();
        }
        {
            const char *interp_types[] = {
                "Wachpress",
                "MeanValue",
                "GaussianRBF",
                "SplineGaussianRBF",
                "DesbrunSplineRBF",
                //"RadialBasis",
            };
            int current = int(interp_mode);
            if (ImGui::ListBox("Interp modes", &current, interp_types, 5, 2)) {
                interp_mode = Curve::InterpMode(current);
                update();
            }
        }


        if (ImGui::InputInt("active_index", &active_weight_index)) {
            update();
        }
        if (ImGui::SliderFloat2("min", bbox.min().data(), -2, 2)) {
            bbox.min() = (bbox.min().array() < bbox.max().array()).select(bbox.min(), bbox.max());
            update();
        }
        if (ImGui::SliderFloat2("max", bbox.max().data(), -2, 2)) {
            bbox.max() = (bbox.min().array() > bbox.max().array()).select(bbox.min(), bbox.max());
            update();
        }
        if (ImGui::SliderFloat("scale", &scale, 0, 2)) {
            _vf_shader.setScale(scale);
        }
        if (edge_drawable) {
            edge_drawable->gui();
        }
        if (face_drawable) {
            face_drawable->gui();
        }
        auto evaluator = curve.evaluator(interp_mode);
        mtao::VecXd coeffs = mtao::VecXd::Unit(curve.points.size(), active_weight_index);
        float v = evaluator.from_coefficients(coeffs, mtao::Vec2d(cursor.x(), cursor.y()))(0);
        ImGui::Text("Cursor Position: (%f,%f):%f", cursor.x(), cursor.y(), v);
    }
    void draw() override {
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::setPointSize(10.);
        camera().draw(gridcolor_drawgroup);
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
                auto &points = curve.points;
                points.emplace_back(EigenIntegration::cast<mtao::Vec2f>(cursor).cast<double>());
                update();
            }
        }
    }

  private:
    Magnum::Shaders::Flat2D _flat_shader;
    Magnum::Shaders::VertexColor2D _vcolor_shader;
    mtao::opengl::VectorFieldShader<2> _vf_shader;

    Magnum::SceneGraph::DrawableGroup2D gridcolor_drawgroup;
    mtao::opengl::objects::Grid<2> grid_mesh;
    mtao::opengl::objects::Mesh<2> curve_mesh;
    mtao::opengl::objects::Mesh<2> curve_points;
    mtao::opengl::objects::Mesh<2> vfield_mesh;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D> *edge_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D> *face_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D> *curve_drawable = nullptr;
    mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D> *curve_point_drawable = nullptr;
    mtao::opengl::MeshDrawable<mtao::opengl::VectorFieldShader<2>> *_vf_viewer = nullptr;
};


MAGNUM_APPLICATION_MAIN(MeshViewer)
