#ifndef WINDOW_H
#define WINDOW_H
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/Trade/AbstractImageConverter.h>
#include <Magnum/GL/Context.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <functional>
#include <Magnum/Image.h>
#include "mtao/hotkey_manager.hpp"
#include <array>
#include "mtao/opengl/objects/types.h"

#include <Magnum/Mesh.h>
#include <Magnum/Math/Vector.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/GL/Version.h>

namespace mtao::opengl {
using namespace Magnum;

class WindowBase : public Magnum::Platform::GlfwApplication {
  public:
    explicit WindowBase(const Arguments &arguments, GL::Version = GL::Version::None);

    void drawEvent() override;

    virtual void draw();
    virtual void gui() = 0;

    void recording_gui();
    Magnum::Image2D current_frame();
    void record_frame_to_file();

    virtual void viewportEvent(ViewportEvent &event) override;

    virtual void keyPressEvent(KeyEvent &event) override;
    virtual void keyReleaseEvent(KeyEvent &event) override;

    virtual void mousePressEvent(MouseEvent &event) override;
    virtual void mouseReleaseEvent(MouseEvent &event) override;
    virtual void mouseMoveEvent(MouseMoveEvent &event) override;
    virtual void mouseScrollEvent(MouseScrollEvent &event) override;
    virtual void textInputEvent(TextInputEvent &event) override;

    bool supportsGeometryShader() const;
    template<typename T>
    bool isExtensionSupported() const;

    void increment_recording_frame_index();
    void reset_recording_frame_index();

    void automatically_increment_recording_frames(bool do_it);

  private:
    Magnum::ImGuiIntegration::Context _imgui{ Magnum::NoCreate };
    Containers::Pointer<Magnum::Trade::AbstractImageConverter> _image_saver = nullptr;
    bool _recording_active = false;
    bool _keep_recording = false;
    bool _recording_includes_gui = false;
    bool _auto_increment = true;
    std::string _recording_filename_format = "output-{:06}.png";
    int _recording_index = 0;
};
class Window2 : public WindowBase {
  public:
    explicit Window2(const Arguments &arguments, GL::Version = GL::Version::None);
    virtual void draw() override;

    virtual void viewportEvent(ViewportEvent &event) override;
    virtual void mousePressEvent(MouseEvent &event) override;
    virtual void mouseReleaseEvent(MouseEvent &event) override;
    virtual void mouseMoveEvent(MouseMoveEvent &event) override;
    virtual void mouseScrollEvent(MouseScrollEvent &event) override;


    Magnum::Vector2 cameraPosition(const Vector2i &position) const;//[0,1]^2 position of the camera
    //position in 2D space with respect to the root node
    Magnum::Vector2 localPosition(const Vector2i &position) const;
    Object2D &root() { return _root; }
    Object2D &scene() { return _scene; }
    Object2D &cameraObject() { return _cameraObject; }
    Magnum::SceneGraph::Camera2D &camera() { return _camera; }
    Magnum::SceneGraph::DrawableGroup2D &drawables() { return _drawables; }
    const Object2D &root() const { return _root; }
    const Object2D &scene() const { return _scene; }
    const Object2D &cameraObject() const { return _cameraObject; }
    const Magnum::SceneGraph::Camera2D &camera() const { return _camera; }
    const Magnum::SceneGraph::DrawableGroup2D &drawables() const { return _drawables; }
    void updateTransformation();

  private:
    Scene2D _scene;
    Object2D _root, _cameraObject;
    Magnum::SceneGraph::Camera2D _camera;
    Magnum::SceneGraph::DrawableGroup2D _drawables;
    Magnum::Vector2 _previousPosition;
    Magnum::Vector2 translation;
    float scale = 1.0;
};

class Window3 : public WindowBase {
  public:
    explicit Window3(const Arguments &arguments, GL::Version = GL::Version::None);
    virtual void draw() override;

    virtual void viewportEvent(ViewportEvent &event) override;
    virtual void mousePressEvent(MouseEvent &event) override;
    virtual void mouseReleaseEvent(MouseEvent &event) override;
    virtual void mouseMoveEvent(MouseMoveEvent &event) override;
    virtual void mouseScrollEvent(MouseScrollEvent &event) override;
    Magnum::Vector3 positionOnSphere(const Magnum::Vector2i &position) const;


    Object3D &root() { return _root; }
    Object3D &scene() { return _scene; }
    Magnum::SceneGraph::Camera3D &camera() { return _camera; }
    Magnum::SceneGraph::DrawableGroup3D &drawables() { return _drawables; }

  private:
    Scene3D _scene;
    Object3D _root, _cameraObject;
    Magnum::SceneGraph::Camera3D _camera;
    Magnum::SceneGraph::DrawableGroup3D _drawables;
    Magnum::Vector3 _previousPosition;
};


template<typename T>
bool WindowBase::isExtensionSupported() const {
    return Magnum::GL::Context::current().isExtensionSupported<T>();
}
}// namespace mtao::opengl

#endif//WINDOW_H
