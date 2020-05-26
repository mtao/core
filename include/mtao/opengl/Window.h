#ifndef WINDOW_H
#define WINDOW_H
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Vector.h>
#include <Magnum/Mesh.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/SceneGraph/Drawable.h>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <array>
#include <functional>

#include "mtao/hotkey_manager.hpp"
#include "mtao/opengl/scene.h"

namespace mtao::opengl {
using namespace Magnum;

class WindowBase : public Magnum::Platform::Application {
   public:
    explicit WindowBase(const Arguments& arguments,
                        GL::Version = GL::Version::None);

    void drawEvent() override;

    virtual void draw();
    virtual void gui() = 0;

    virtual void viewportEvent(ViewportEvent& event) override;

    virtual void keyPressEvent(KeyEvent& event) override;
    virtual void keyReleaseEvent(KeyEvent& event) override;

    virtual void mousePressEvent(MouseEvent& event) override;
    virtual void mouseReleaseEvent(MouseEvent& event) override;
    virtual void mouseMoveEvent(MouseMoveEvent& event) override;
    virtual void mouseScrollEvent(MouseScrollEvent& event) override;
    virtual void textInputEvent(TextInputEvent& event) override;

    bool supportsGeometryShader() const;
    template <typename T>
    bool isExtensionSupported() const;

   private:
    Magnum::ImGuiIntegration::Context _imgui{Magnum::NoCreate};
};

class Window : public WindowBase {
   public:
    explicit Window(const Arguments& arguments,
                    GL::Version = GL::Version::None);
    explicit Window(char mode, const Arguments& arguments,
                    GL::Version = GL::Version::None);
    virtual void draw() = 0;
    virtual void gui() = 0;
    void drawEvent() override;
    virtual void viewportEvent(ViewportEvent& event) override;
    virtual void mousePressEvent(MouseEvent& event) override;
    virtual void mouseReleaseEvent(MouseEvent& event) override;
    virtual void mouseMoveEvent(MouseMoveEvent& event) override;
    virtual void mouseScrollEvent(MouseScrollEvent& event) override;
    // if hte mode is 2 or 3, then only 2d or 3d events are emitted respectively
    void set_mode(char mode) { _scene_mode = mode; }
    char get_mode() const { return _scene_mode; }

   private:
    char _scene_mode = 0;
};

class Window2 : public WindowBase {
   public:
    explicit Window2(const Arguments& arguments,
                     GL::Version = GL::Version::None);
    virtual void draw() override;

    virtual void viewportEvent(ViewportEvent& event) override;
    virtual void mousePressEvent(MouseEvent& event) override;
    virtual void mouseReleaseEvent(MouseEvent& event) override;
    virtual void mouseMoveEvent(MouseMoveEvent& event) override;
    virtual void mouseScrollEvent(MouseScrollEvent& event) override;

   private:
    Magnum::SceneGraph::DrawableGroup2D _drawables;
    Magnum::Vector2 _previousPosition;
};

class Window3 : public WindowBase {
   public:
    explicit Window3(const Arguments& arguments,
                     GL::Version = GL::Version::None);
    virtual void draw() override;

    virtual void viewportEvent(ViewportEvent& event) override;
    virtual void mousePressEvent(MouseEvent& event) override;
    virtual void mouseReleaseEvent(MouseEvent& event) override;
    virtual void mouseMoveEvent(MouseMoveEvent& event) override;
    virtual void mouseScrollEvent(MouseScrollEvent& event) override;

   private:
    Magnum::SceneGraph::DrawableGroup3D _drawables;
    Magnum::Vector3 _previousPosition;
};

template <typename T>
bool WindowBase::isExtensionSupported() const {
    return Magnum::GL::Context::current().isExtensionSupported<T>();
}
}  // namespace mtao::opengl

#endif  // WINDOW_H
