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

#include <glm/gtc/matrix_transform.hpp> 
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



class MeshViewer: public mtao::opengl::Window2 {
    public:

    MeshViewer(const Arguments& args): Window2(args) {
        Corrade::Utility::Arguments myargs;
        myargs.addArgument("filename").parse(args.argc,args.argv);
        std::string filename = myargs.value("filename");
        auto [V,F] = mtao::geometry::mesh::read_objF(filename);
        auto bb = mtao::geometry::bounding_box(V);
        mtao::Vec3f mean = (bb.min() + bb.max())/2;
        V.colwise() -= mean;
        mesh.setTriangleBuffer(V.topRows<2>(),F.cast<unsigned int>());
        auto E = mtao::geometry::mesh::boundary_facets(F);

        mesh.setEdgeBuffer(E.cast<unsigned int>());



        drawable = new mtao::opengl::Drawable<Magnum::Shaders::Flat2D>{mesh,flat_shader, drawables()};
        drawable->activate_triangles({});
        drawable->activate_edges();
        mesh.setParent(&root());

    }
    void gui() override {
        gui_func();
        if(drawable) {
            drawable->gui();
        }
    }
    void draw() override {
        Window2::draw();
    }
    private:
    Magnum::Shaders::Flat2D flat_shader;
    mtao::opengl::objects::Mesh<2> mesh;
    mtao::opengl::Drawable<Magnum::Shaders::Flat2D>* drawable = nullptr;


};




MAGNUM_APPLICATION_MAIN(MeshViewer)
