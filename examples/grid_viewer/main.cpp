#include "mtao/opengl/Window.h"
#include <iostream>
#include <Eigen/Geometry>
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

using namespace mtao::opengl;


class MeshViewer: public mtao::opengl::Window3 {
    public:
        enum class Mode: int { Smoothing, LSReinitialization };
        Mode mode = Mode::LSReinitialization;

        float permeability = 100.0;
        float timestep = 1000.0;
        bool animate = false;
        using Vec = mtao::VectorX<GLfloat>;
        using Vec3 = mtao::Vec3f;
        Vec data;
        Vec data_original;
        Vec dx;
        Vec dy;
        Vec signs;
        Eigen::AlignedBox<float,3> bbox;
        Eigen::SparseMatrix<float> L;

        std::array<int,3> N{{20,20,20}};
        int& NI=N[0];
        int& NJ=N[1];
        int& NK=N[2];

        Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;

        MeshViewer(const Arguments& args): Window3(args), _wireframe_shader{Magnum::Shaders::MeshVisualizer::Flag::Wireframe} {
            bbox.min().setConstant(-1);
            bbox.max().setConstant(1);
            mv_drawable = new mtao::opengl::Drawable<Magnum::Shaders::MeshVisualizer>{grid,_wireframe_shader, drawables()};
            mv_drawable->set_visibility(false);;


            edge_drawable = new mtao::opengl::Drawable<Magnum::Shaders::Flat3D>{grid,_flat_shader, drawables()};
            edge_drawable->activate_triangles({});
            edge_drawable->activate_edges();
            grid.setParent(&root());
            update();
        }
        void update() {
            //mtao::geometry::grid::Grid3f g(std::array<int,3>{{NI,NJ,NK}});
            auto g = mtao::geometry::grid::Grid3f::from_bbox
                (bbox, std::array<int,3>{{NI,NJ,NK}});

            grid.set(g);


        }
        void do_animation() {
        }
        void gui() override {
            if(ImGui::InputInt3("N", &NI))  {
                update();
            }
            if(ImGui::SliderFloat3("min", bbox.min().data(),-2,2))  {
                bbox.min() = (bbox.min().array() < bbox.max().array()).select(bbox.min(),bbox.max());
                update();
            }
            if(ImGui::SliderFloat3("max", bbox.max().data(),-2,2))  {
                bbox.max() = (bbox.min().array() > bbox.max().array()).select(bbox.min(),bbox.max());
                update();
            }
            if(mv_drawable) {
                mv_drawable->gui();
            }
            if(edge_drawable) {
                edge_drawable->gui();
            }
            if(ImGui::Button("Step")) {
                do_animation();
            }
        }
        void draw() override {
            if(animate) {
                do_animation();
            }
            Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
            Window3::draw();
        }
    private:
        Magnum::Shaders::MeshVisualizer _wireframe_shader;
        Magnum::Shaders::Flat3D _flat_shader;
        mtao::opengl::objects::Grid<3> grid;
        mtao::opengl::Drawable<Magnum::Shaders::MeshVisualizer>* mv_drawable = nullptr;
        mtao::opengl::Drawable<Magnum::Shaders::Flat3D>* edge_drawable = nullptr;


};




MAGNUM_APPLICATION_MAIN(MeshViewer)
