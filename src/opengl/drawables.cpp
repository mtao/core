#include "mtao/opengl/drawables.h"
#include <imgui.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <iostream>

namespace mtao::opengl {


    template <>
        void Drawable<Shaders::Flat2D>::gui(const std::string& name_) {

            std::string name = name_;
            if(name.empty()) {
                name = "Flat Shader";
            }
            if(ImGui::TreeNode(name.c_str())) {
                ImGui::Checkbox("Visible", &visible);
                ImGui::ColorEdit4("Color", _data.color.data());
                ImGui::TreePop();
            }
        }


    template <>
        void Drawable<Shaders::VertexColor2D>::gui(const std::string& name_) {
            std::string name = name_;
            if(name.empty()) {
                name = "Vertex Color Shader";
            }
            if(ImGui::TreeNode(name.c_str())) {
                ImGui::Checkbox("Visible", &visible);
                ImGui::TreePop();
            }
        }
    template <>
        void Drawable<Shaders::Flat2D>::set_buffers() {
            _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::Flat2D::Position{});
            _shader.setColor(_data.color);
            if(_shader.flags() & Shaders::Flat2D::Flag::AlphaMask) {
                _shader.setAlphaMask(_data.alpha_mask);
            }
        }
    template <>
        void Drawable<Shaders::VertexColor2D>::set_buffers() {
            _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::VertexColor2D::Position{});
            _mesh.addVertexBuffer(_mesh.color_buffer, 0, Shaders::VertexColor2D::Color4{});
        }

    template <>
        void Drawable<Shaders::Flat3D>::gui(const std::string& name_) {
            std::string name = name_;
            if(name.empty()) {
                name = "Flat Shader";
            }
            if(ImGui::TreeNode(name.c_str())) {
                ImGui::Checkbox("Visible", &visible);
                ImGui::ColorEdit4("Color", _data.color.data());
                ImGui::TreePop();
            }
        }
    template <>
        void Drawable<Shaders::VertexColor3D>::gui(const std::string& name_) {
            std::string name = name_;
            if(name.empty()) {
                name = "Vertex Color Shader";
            }
            if(ImGui::TreeNode(name.c_str())) {
                ImGui::Checkbox("Visible", &visible);
                ImGui::TreePop();
            }
        }
    template <>
        void Drawable<Shaders::Phong>::gui(const std::string& name_) {
            std::string name = name_;
            if(name.empty()) {
                name = "Phong Shader";
            }
            if(ImGui::TreeNode(name.c_str())) {
                ImGui::Checkbox("Visible", &visible);
                ImGui::ColorEdit4("Ambient Color", _data.ambient_color.data());
                ImGui::ColorEdit4("Diffuse Color", _data.diffuse_color.data());
                ImGui::ColorEdit4("Specular Color", _data.specular_color.data());

                ImGui::SliderFloat("Shininess", &_data.shininess, 0., 100.);
                {
                    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
                    if(ImGui::BeginTabBar("Lights", tab_bar_flags)) {
                        for(size_t i = 0; i < _data.light_positions.size(); ++i) {

                            std::stringstream ss;
                            ss << i;
                            if(ImGui::BeginTabItem(ss.str().c_str())) {
                                ImGui::SliderFloat3("Light Position", _data.light_positions[i].data(),-100.,100.);
                                ImGui::ColorEdit4("Light Color", _data.light_colors[i].data());
                                ImGui::EndTabItem();
                            }
                        }
                        ImGui::EndTabBar();
                    }
                }
                ImGui::TreePop();
            }
            //if(_shader.flags() & Shaders::Phong::Flag::AlphaMask) {
            //    _shader.setAlphaMask(_data.alpha_mask);
            //}
        }
    template <>
        void Drawable<Shaders::MeshVisualizer>::gui(const std::string& name_) {
            std::string name = name_;
            if(name.empty()) {
                name = "Mesh Visualizer Shader";
            }
            if(ImGui::TreeNode(name.c_str())) {
                ImGui::Checkbox("Visible", &visible);
                ImGui::ColorEdit4("Color", _data.color.data());
                ImGui::ColorEdit4("Wireframe Color", _data.wireframe_color.data());
                ImGui::SliderFloat("Wireframe Width", &_data.wireframe_width,0.,10.);
                ImGui::SliderFloat("Smoothness", &_data.smoothness,0.,10.);
                ImGui::TreePop();
            }
        }
    template <>
        void Drawable<Shaders::Flat3D>::set_buffers() {
            _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::Flat3D::Position{});
            _shader.setColor(_data.color);
            if(_shader.flags() & Shaders::Flat3D::Flag::AlphaMask) {
                _shader.setAlphaMask(_data.alpha_mask);
            }
        }
    template <>
        void Drawable<Shaders::VertexColor3D>::set_buffers() {
            _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::VertexColor3D::Position{});
            _mesh.addVertexBuffer(_mesh.color_buffer, 0, Shaders::VertexColor3D::Color4{});
        }
    template <>
        void Drawable<Shaders::Phong>::set_buffers() {
            _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::Phong::Position{});
            _mesh.addVertexBuffer(_mesh.normal_buffer, 0, Shaders::Phong::Normal{});
            _shader.setShininess(_data.shininess)
                .setDiffuseColor(_data.diffuse_color)
                .setAmbientColor(_data.ambient_color)
                .setSpecularColor(_data.specular_color)
                .setLightPositions(_data.light_positions)
                .setLightColors(_data.light_colors);
            if(_shader.flags() & Shaders::Phong::Flag::AlphaMask) {
                _shader.setAlphaMask(_data.alpha_mask);
            }
        }
    template <>
        void Drawable<Shaders::MeshVisualizer>::set_buffers() {
            _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::MeshVisualizer::Position{});
            _shader.setColor(_data.color)
                .setWireframeColor(_data.wireframe_color)
                .setWireframeWidth(_data.wireframe_width)
                .setSmoothness(_data.smoothness);

            if(_data.viewport_size) {
                _shader.setViewportSize(*_data.viewport_size);
            } else {
                _shader.setViewportSize(Magnum::Vector2{GL::defaultFramebuffer.viewport().size()});
            }
        }
    template <>
        void Drawable<Shaders::Phong>::set_matrices(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
            _shader.setTransformationMatrix(transformationMatrix)
                .setNormalMatrix(transformationMatrix.rotationScaling())
                .setProjectionMatrix(camera.projectionMatrix());
        }
    template <>
            bool Drawable<Shaders::VertexColor2D>::lint_buffers()  {
                bool ret = lint_vertex();
                if(edge_primitive) { ret &= lint_edge(); }
                if(triangle_primitive) { ret &= lint_triangle(); }
                ret &= lint_color();
                return ret;
            }
    template <>
            bool Drawable<Shaders::VertexColor3D>::lint_buffers()  {
                bool ret = lint_vertex();
                if(edge_primitive) { ret &= lint_edge(); }
                if(triangle_primitive) { ret &= lint_triangle(); }
                ret &= lint_color();
                return ret;
            }
    template <>
            bool Drawable<Shaders::Phong>::lint_buffers()  {
                bool ret = lint_vertex();
                if(edge_primitive) { ret &= lint_edge(); }
                if(triangle_primitive) { ret &= lint_triangle(); }
                ret &= lint_normal();
                return ret;
            }
}
