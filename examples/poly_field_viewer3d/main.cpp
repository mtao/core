
#include "mtao/opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include <memory>
#include <numeric>
#include <algorithm>
#include "mtao/geometry/mesh/boundary_facets.h"
#include "mtao/geometry/mesh/read_obj.hpp"
#include "mtao/geometry/bounding_box.hpp"
#include "mtao/opengl/drawables.h"
#include <mtao/types.h>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayView.h>

#include <Corrade/Utility/Arguments.h>
#include "mtao/opengl/objects/partitioned_polynomial_shaded_mesh.hpp"
#include "mtao/opengl/shaders/polynomial_scalar_field.hpp"

bool animate = false;
bool save_frame = false;
ImVec4 clear_color = ImColor(114, 144, 154);


class MeshViewer : public mtao::opengl::Window3 {
  public:
    bool show_bbox_faces = false;
    bool show_bbox_edges = true;

    MeshViewer(const Arguments &args) : Window3(args), pmesh(&drawables()) {
        //MeshViewer(const Arguments& args): Window3(args,Magnum::GL::Version::GL210), _wireframe_shader{} {
        Corrade::Utility::Arguments myargs;
        myargs.addArgument("filename").parse(args.argc, args.argv);
        std::string filename = myargs.value("filename");
        auto [V, F] = mtao::geometry::mesh::read_objF(filename);
        auto bb = mtao::geometry::bounding_box(V);
        mtao::Vec3f mean = (bb.min() + bb.max()) / 2;
        V.colwise() -= mean;

        pmesh.setTriangleBuffer(V, F.cast<unsigned int>());
        std::vector<int> offsets(F.cols());
        //std::iota(offsets.begin(),offsets.end(), 0);
        offsets.resize(3);
        offsets[0] = 0;
        offsets[1] = F.cols() / 2;
        offsets[2] = F.cols();
        pmesh.setParent(&root());
        pmesh.setCount(pmesh.triangle_Count);

        pmesh.set_offsets(offsets);
    }
    void gui() override {
        pmesh.gui();
    }
    void draw() override {
        Window3::draw();
    }

  private:
    mtao::opengl::objects::PartitionedPolynomialShadedMesh<3> pmesh;
};


MAGNUM_APPLICATION_MAIN(MeshViewer)
