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

#include <mtao/eigen/stack.h>
#include <memory>
#include <algorithm>

#include <mtao/eigen_utils.h>
#include<Eigen/IterativeLinearSolvers>
using namespace Magnum::Math::Literals;



using namespace mtao::opengl;


class MeshViewer: public mtao::opengl::Window2 {
    public:
        enum class Mode: int { Smoothing, LSReinitialization };
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
        Eigen::SparseMatrix<float> L;

        std::array<int,2> N{{20,20}};
        int& NI=N[0];
        int& NJ=N[1];

        Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;

        MeshViewer(const Arguments& args): Window2(args) {


            edge_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{grid,_flat_shader, drawables()};
            edge_drawable->activate_triangles({});
            edge_drawable->activate_edges();
            //face_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D>{grid,_vcolor_shader, drawables()};
            grid.setParent(&root());
            cursor_mesh.setParent(&scene());
            cursor_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{cursor_mesh,_flat_shader, drawables()};
            cursor_mesh.setVertexBuffer(mtao::Vec2f::Zero().eval());
            cursor_drawable->data().color = 0xffffff_rgbf;
            visible_grid.setParent(&scene());
            visible_drawable = new mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>{visible_grid,_flat_shader, drawables()};
            visible_drawable->data().color = 0xff0000_rgbf;
            visible_drawable->deactivate();
            visible_drawable->activate_edges();
            update();
        }
        void gui() override {}
        void update() {
            Eigen::AlignedBox<float,2> bbox;
            bbox.min().setConstant(-1);
            bbox.max().setConstant(1);
            auto g = mtao::geometry::grid::Grid2f::from_bbox
                (bbox, std::array<int,2>{{10,10}});

            grid.set(g);
            cursor_drawable->deactivate();
            cursor_drawable->activate_points();
            {
                auto g = mtao::geometry::grid::Grid2f(std::array<int,2>{{2,2}});

                //auto V = g.vertices();
                //mtao::RowVectorX<float> C = V.colwise()
                visible_grid.set(g);
            }


        }
        void draw() override {
            Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
            Magnum::GL::Renderer::setPointSize(10.);
            Window2::draw();
        }
        void mouseMoveEvent(MouseMoveEvent& event) override{
            Window2::mouseMoveEvent(event);
            {
                auto cursor = localPosition(event.position());
                cursor_mesh.setVertexBuffer( cursor);;

            }
        }

    private:
        Magnum::Shaders::Flat2D _flat_shader;
        Magnum::Shaders::VertexColor2D _vcolor_shader;
        mtao::opengl::objects::Grid<2> grid;
        mtao::opengl::objects::Mesh<2> cursor_mesh;
        mtao::opengl::objects::Grid<2> visible_grid;
        mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>* edge_drawable = nullptr;
        mtao::opengl::MeshDrawable<Magnum::Shaders::VertexColor2D>* face_drawable = nullptr;
        mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>* cursor_drawable = nullptr;
        mtao::opengl::MeshDrawable<Magnum::Shaders::Flat2D>* visible_drawable = nullptr;


};




MAGNUM_APPLICATION_MAIN(MeshViewer)
