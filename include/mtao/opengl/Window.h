#ifndef WINDOW_H
#define WINDOW_H
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/GL/Context.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <functional>
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

class WindowBase: public Magnum::Platform::Application {
    public:

        explicit WindowBase(const Arguments& arguments, GL::Version = GL::Version::None);

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
class Window2: public WindowBase {
    public:
        explicit Window2(const Arguments& arguments, GL::Version = GL::Version::None);
        virtual void draw() override;
        virtual void gui() = 0;
        
        virtual void viewportEvent(ViewportEvent& event) override;
        virtual void mousePressEvent(MouseEvent& event) override;
        virtual void mouseReleaseEvent(MouseEvent& event) override;
        virtual void mouseMoveEvent(MouseMoveEvent& event) override;
        virtual void mouseScrollEvent(MouseScrollEvent& event) override;


        Magnum::Vector2 cameraPosition(const Vector2i& position) const;//[0,1]^2 position of the camera
        //position in 2D space with respect to the root node
        Magnum::Vector2 localPosition(const Vector2i& position) const;
        Object2D& root() { return _root; }
        Object2D& scene() { return _scene; }
        Object2D& cameraObject() { return _cameraObject; }
        Magnum::SceneGraph::Camera2D& camera() { return _camera; }
        Magnum::SceneGraph::DrawableGroup2D& drawables() { return _drawables; }
        const Object2D& root() const { return _root; }
        const Object2D& scene() const { return _scene; }
        const Object2D& cameraObject() const { return _cameraObject; }
        const Magnum::SceneGraph::Camera2D& camera() const { return _camera; }
        const Magnum::SceneGraph::DrawableGroup2D& drawables() const { return _drawables; }
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

class Window3: public WindowBase {
    public:
        explicit Window3(const Arguments& arguments, GL::Version = GL::Version::None);
        virtual void draw() override;
        virtual void gui() = 0;
        
        virtual void viewportEvent(ViewportEvent& event) override;
        virtual void mousePressEvent(MouseEvent& event) override;
        virtual void mouseReleaseEvent(MouseEvent& event) override;
        virtual void mouseMoveEvent(MouseMoveEvent& event) override;
        virtual void mouseScrollEvent(MouseScrollEvent& event) override;
        Magnum::Vector3 positionOnSphere(const Magnum::Vector2i& position) const;


        Object3D& root() { return _root; }
        Object3D& scene() { return _scene; }
        Magnum::SceneGraph::Camera3D& camera() { return _camera; }
        Magnum::SceneGraph::DrawableGroup3D& drawables() { return _drawables; }

    private:
        Scene3D _scene;
        Object3D _root, _cameraObject;
        Magnum::SceneGraph::Camera3D _camera;
        Magnum::SceneGraph::DrawableGroup3D _drawables;
        Magnum::Vector3 _previousPosition;


};


template <typename T>
bool WindowBase::isExtensionSupported() const {
    return Magnum::GL::Context::current().isExtensionSupported<T>();
}
}

#endif//WINDOW_H
