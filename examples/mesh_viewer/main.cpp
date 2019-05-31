#include "mtao/opengl/Window.h"
#include <iostream>
#include "imgui.h"
#include "mtao/opengl/shader.h"
#include "mtao/opengl/vao.h"
#include "mtao/opengl/bo.h"
#include <memory>
#include <algorithm>
#include "mtao/opengl/renderers/mesh.h"
#include "mtao/opengl/renderers/bbox.h"
#include "mtao/geometry/mesh/sphere.hpp"
#include "mtao/geometry/mesh/read_obj.hpp"
#include "mtao/geometry/bounding_box.hpp"
#include "mtao/geometry/mesh/vertex_normals.hpp"
#include "mtao/opengl/camera.hpp"
#include <mtao/types.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayView.h>

#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/Phong.h>
#include <Corrade/Utility/Arguments.h>

#include <glm/gtc/matrix_transform.hpp> 
bool animate = false;
bool save_frame = false;
ImVec4 clear_color = ImColor(114, 144, 154);

/*
using namespace mtao::opengl;
Camera3D cam;


using ColVectors3f = mtao::ColVectors<GLfloat,3>;
using ColVectors3i = mtao::ColVectors<GLuint,3>;


std::unique_ptr<Window> window;
std::unique_ptr<renderers::MeshRenderer> renderer;
std::unique_ptr<renderers::BBoxRenderer3> bbox_renderer;

void prepare_mesh(const ColVectors3f& V, const ColVectors3i&F) {
    mtao::logging::debug() << "preparing mesh. f range: " << F.minCoeff() << " << " << F.maxCoeff();
    mtao::logging::debug() << "preparing mesh. v count: " << V.cols();
    renderer = std::make_unique<renderers::MeshRenderer>(3);
    bbox_renderer = std::make_unique<renderers::BBoxRenderer3>();


    auto bb = mtao::geometry::bounding_box(V);

    float scale = bb.sizes().maxCoeff();

    renderer->setMesh(V,F,false);
    auto m = ((bb.min() + bb.max())/2).eval();
    cam.target_pos() = glm::vec3(m.x(),m.y(),m.z());
    cam.camera_pos() = glm::vec3(m.x(),m.y(),m.z() + 5);
    cam.update();
    bbox_renderer->set(bb);

    renderers::MeshRenderer::MatrixXgf C = renderer->computeNormals(V,F).array();
    renderer->setColor(C);
}
void set_mvp(int w, int h) {
    cam.set_shape(w,h);
    cam.pan();
    cam.update();
}

*/
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

void render(int width, int height) {
    
    /*
    // Rendering
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

    set_mvp(width,height);


    renderer->set_mvp(cam.mvp());
    renderer->set_mvp(cam.mv(),cam.p());
    bbox_renderer->set_mvp(cam.mvp());
    bbox_renderer->set_mvp(cam.mv(),cam.p());

    renderer->render();
    bbox_renderer->render();
    if(save_frame) {
        window->save_frame("frame.png");
        save_frame = false;
    }
    */



}

using namespace Magnum;
using namespace Math::Literals;


class ColoredDrawable: public SceneGraph::Drawable3D {
    public:
        using Object3D = mtao::opengl::Window3::Object3D;
          explicit ColoredDrawable(Object3D& object, Shaders::Phong& shader, GL::Mesh& mesh, const Color4& color, SceneGraph::DrawableGroup3D& group): SceneGraph::Drawable3D{object, &group}, _shader(shader), _mesh(mesh), _color{color} {}

    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

        Shaders::Phong& _shader;
        GL::Mesh& _mesh;
        Color4 _color;
};

void ColoredDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
    _shader
        .setDiffuseColor(_color)
        .setLightPosition(camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}))
        .setTransformationMatrix(transformationMatrix)
        .setNormalMatrix(transformationMatrix.rotationScaling())
        .setProjectionMatrix(camera.projectionMatrix());

    _mesh.draw(_shader);
}

class MeshViewer: public mtao::opengl::Window3 {
    public:

    MeshViewer(const Arguments& args): Window3(args) {
        Utility::Arguments myargs;
        myargs.addArgument("filename").parse(args.argc,args.argv);
        std::string filename = myargs.value("filename");
        auto [V,F] = mtao::geometry::mesh::read_objF(filename);
        auto bb = mtao::geometry::bounding_box(V);
        mtao::Vec3f mean = (bb.min() + bb.max())/2;
        V.colwise() -= mean;

        mtao::ColVectors<float,6> VN(6,V.cols());
        VN.topRows<3>() = V;
        VN.bottomRows<3>() = mtao::geometry::mesh::vertex_normals(V,F);
        vbuffer.setData(Containers::ArrayView<const float>{VN.data(),VN.size()});
        Containers::Array<char> indexData;
        MeshIndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::vector<unsigned int> inds(F.data(),F.data()+F.size());

        std::tie(indexData, indexType, indexStart, indexEnd) =
            MeshTools::compressIndices(inds);
        ibuffer.setData(indexData);

        _coloredShader
            .setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0xffffff_rgbf)
            .setShininess(80.0f);

        mesh.setPrimitive(GL::MeshPrimitive::Triangles)
            .setCount(inds.size())
            .addVertexBuffer(vbuffer, 0,
                Shaders::Phong::Position{}, Shaders::Phong::Normal{})
            .setIndexBuffer(ibuffer,0,indexType,indexStart,indexEnd);


        new ColoredDrawable{manipulator(), _coloredShader, mesh, 0xffffff_rgbf, drawables()};

    }
    void gui() override {
        gui_func();
    }
    void draw() override {
        Window3::draw();
    }
    private:
    GL::Buffer vbuffer, ibuffer;
    GL::Mesh mesh;
    Shaders::Phong _coloredShader;

};


/*
int main(int argc, char * argv[]) {

    set_opengl_version_hints();
    window = std::make_unique<Window>();
    window->set_gui_func(gui_func);
    window->set_render_func(render);
    window->makeCurrent();

    mtao::ColVectors<GLfloat,3> V;
    mtao::ColVectors<int,3> F;
    if(argc < 2) {
        std::cout << "Need an obj input!" << std::endl;
        std::cout << "Loading a sphere mesh instead" << std::endl;
        std::tie(V,F) = mtao::geometry::mesh::sphere<GLfloat>(3);
    } else {
        std::tie(V,F) = mtao::geometry::mesh::read_objF(argv[1]);
    }
    prepare_mesh(V,F.cast<GLuint>());

    animate = true;
    */
    /*
    window->record([&](int frame) -> bool {
            return frame < 20;
            }, "frame");
            */
/*
    window->run();

    exit(EXIT_SUCCESS);
}
*/


MAGNUM_APPLICATION_MAIN(MeshViewer)
