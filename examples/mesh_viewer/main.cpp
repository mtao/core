#include "mtao/opengl/Window.h"
#include <iostream>
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
#include <Magnum/Shaders/Phong.h>
#include <Corrade/Utility/Arguments.h>
#include "mtao/opengl/objects/mesh.h"
#include "mtao/opengl/objects/bbox.h"
#include <mtao/geometry/mesh/vertex_normals.hpp>
#include <mtao/geometry/mesh/shapes/vector_field.hpp>

bool animate = false;
bool save_frame = false;
ImVec4 clear_color = ImColor(114, 144, 154);

void gui_func() {
    {

        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Checkbox("Animate",&animate);
        ImGui::Checkbox("Store",&save_frame);

        //renderer->imgui_interface();
    }
    if(ImGui::Button("Reset  Camera?")) {
        //cam.reset();
    }

}



class MeshViewer: public mtao::opengl::Window3 {
    public:
        bool show_bbox_faces = false;
        bool show_bbox_edges = true;

        MeshViewer(const Arguments& args): Window3(args), _wireframe_shader{supportsGeometryShader()?Magnum::Shaders::MeshVisualizer::Flag::Wireframe:Magnum::Shaders::MeshVisualizer::Flag{}} {
    //MeshViewer(const Arguments& args): Window3(args,Magnum::GL::Version::GL210), _wireframe_shader{} {
        Corrade::Utility::Arguments myargs;
        myargs.addArgument("filename").parse(args.argc,args.argv);
        std::string filename = myargs.value("filename");
        auto [V,F] = mtao::geometry::mesh::read_objF(filename);
        auto bb = mtao::geometry::bounding_box(V);
        mtao::Vec3f mean = (bb.min() + bb.max())/2;
        V.colwise() -= mean;
        mesh.setTriangleBuffer(V,F.cast<unsigned int>());
        auto E = mtao::geometry::mesh::boundary_facets(F);

        bb = mtao::geometry::bounding_box(V);
        bbox.set_bbox(bb);

        mesh.setEdgeBuffer(E.cast<unsigned int>());

        auto N = mtao::geometry::mesh::vertex_normals(V,F);
        mtao::ColVecs4f COL(4,V.cols());
        COL.topRows<3>()= N.cast<float>();
        COL.row(3).setConstant(1);

        mesh.setColorBuffer(COL);

        {
            double edge_length_sum = 0;
            for(int i = 0; i < E.cols(); ++i) {
                auto e = E.col(i);
                double d = (V.col(e(0)) - V.col(e(1))).norm();
                edge_length_sum += d;
            }
            double mean_edge_length = edge_length_sum / E.cols();
            
            std::cout << "Normal cols: " << N.cols() << std::endl;
            auto [nV,nF] = mtao::geometry::mesh::shapes::vector_field(V,(mean_edge_length * N).eval(),.1 * mean_edge_length);
            std::cout << nV.rows() << " " << nV.cols() << std::endl;
            std::cout << nF.rows() << " " << nF.cols() << std::endl;
            normal_mesh.setTriangleBuffer(nV,nF.cast<unsigned int>());
        }

    using namespace Magnum;
    using namespace Math::Literals;

        vphong_drawable = new mtao::opengl::Drawable<Magnum::Shaders::Phong>{mesh,_vcolor_shader, drawables()};

        vphong_drawable->data().ambient_color = 0x111111_rgbf;
        vphong_drawable->data().diffuse_color = 0xffffff_rgbf;
        vphong_drawable->data().specular_color = 0xffffff_rgbf;


        phong_drawable = new mtao::opengl::Drawable<Magnum::Shaders::Phong>{normal_mesh,_shader, drawables()};
        mv_drawable = new mtao::opengl::Drawable<Magnum::Shaders::MeshVisualizer>{mesh,_wireframe_shader, drawables()};
        normal_mesh.setParent(&root());


        edge_drawable = new mtao::opengl::Drawable<Magnum::Shaders::Flat3D>{mesh,_flat_shader, drawables()};
        edge_drawable->deactivate();
        edge_drawable->activate_edges();

        point_drawable = new mtao::opengl::Drawable<Magnum::Shaders::Flat3D>{mesh,_flat_shader, drawables()};
        point_drawable->deactivate();
        point_drawable->activate_points();
        mesh.setParent(&root());

        bbox_drawable = mtao::opengl::make_drawable(bbox,_flat_shader,drawables());
        bbox.setParent(&root());
        bbox_drawable->deactivate();
        bbox_drawable->activate_edges();



    }
    void gui() override {
        gui_func();
        if(mv_drawable) {
            mv_drawable->gui();
        }
        if(phong_drawable) {
            phong_drawable->gui();
        }
        if(vphong_drawable) {
            vphong_drawable->gui("VColor Phong");
        }
        if(edge_drawable) {
            edge_drawable->gui("Edge");
        }
        if(point_drawable) {
            point_drawable->gui("Point");
        }

        if(ImGui::Checkbox("BBox Faces", &show_bbox_faces)) {
            if(show_bbox_faces) {
                bbox_drawable->activate_triangles();
            } else {
                bbox_drawable->activate_triangles({});
            }
        }
        if(ImGui::Checkbox("BBox Edges", &show_bbox_edges)) {
            if(show_bbox_edges) {
                bbox_drawable->activate_edges();
            } else {
                bbox_drawable->activate_edges({});
            }
        }
    }
    void draw() override {
        Window3::draw();
    }
    private:
    Magnum::Shaders::Phong _shader;
    Magnum::Shaders::Phong _vcolor_shader = Magnum::Shaders::Phong(Magnum::Shaders::Phong::Flag::VertexColor);
    Magnum::Shaders::MeshVisualizer _wireframe_shader;
    Magnum::Shaders::Flat3D _flat_shader;
    mtao::opengl::objects::Mesh<3> mesh;
    mtao::opengl::objects::Mesh<3> normal_mesh;
    mtao::opengl::objects::BoundingBox<3> bbox;
    mtao::opengl::Drawable<Magnum::Shaders::Phong>* phong_drawable = nullptr;
    mtao::opengl::Drawable<Magnum::Shaders::Phong>* vphong_drawable = nullptr;
    mtao::opengl::Drawable<Magnum::Shaders::MeshVisualizer>* mv_drawable = nullptr;
    mtao::opengl::Drawable<Magnum::Shaders::Flat3D>* edge_drawable = nullptr;
    mtao::opengl::Drawable<Magnum::Shaders::Flat3D>* point_drawable = nullptr;
    mtao::opengl::Drawable<Magnum::Shaders::Flat3D>* bbox_drawable = nullptr;


};




MAGNUM_APPLICATION_MAIN(MeshViewer)
