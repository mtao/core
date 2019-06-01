#ifndef WINDOW_H
#define WINDOW_H
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <functional>
#include "mtao/hotkey_manager.hpp"
#include <array>
#include "mtao/opengl/objects/types.h"

#include <Magnum/Mesh.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>

namespace mtao::opengl {

class Window: public Magnum::Platform::Application {
    public:

        explicit Window(const Arguments& arguments);

        void drawEvent() override;

        virtual void draw() = 0;
        virtual void gui() = 0;

        virtual void viewportEvent(ViewportEvent& event) override;

        virtual void keyPressEvent(KeyEvent& event) override;
        virtual void keyReleaseEvent(KeyEvent& event) override;

        virtual void mousePressEvent(MouseEvent& event) override;
        virtual void mouseReleaseEvent(MouseEvent& event) override;
        virtual void mouseMoveEvent(MouseMoveEvent& event) override;
        virtual void mouseScrollEvent(MouseScrollEvent& event) override;
        virtual void textInputEvent(TextInputEvent& event) override;


    private:
        Magnum::ImGuiIntegration::Context _imgui{Magnum::NoCreate};

};

class Window3: public Window {
    public:
        explicit Window3(const Arguments& arguments);
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
}

#endif//WINDOW_H
