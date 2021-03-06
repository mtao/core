#include "mtao/opengl/drawables.h"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <imgui.h>

#include <iostream>

namespace mtao::opengl {

template<>
void DrawableBase<Shaders::Flat2D>::gui(const std::string &name_) {
    std::string name = name_;
    if (name.empty()) {
        name = "Flat Shader";
    }
    if (ImGui::TreeNode(name.c_str())) {
        ImGui::Checkbox("Visible", &visible);
        ImGui::ColorEdit4("Color", data().color.data());
        ImGui::TreePop();
    }
}

template<>
void DrawableBase<Shaders::VertexColor2D>::gui(const std::string &name_) {
    std::string name = name_;
    if (name.empty()) {
        name = "Vertex Color Shader";
    }
    if (ImGui::TreeNode(name.c_str())) {
        ImGui::Checkbox("Visible", &visible);
        ImGui::TreePop();
    }
}
template<>
void MeshDrawable<Shaders::Flat2D>::set_buffers() {
    _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::Flat2D::Position{});
    shader().setColor(data().color);
    if (shader().flags() & Shaders::Flat2D::Flag::AlphaMask) {
        shader().setAlphaMask(data().alpha_mask);
    }
}
template<>
void MeshDrawable<Shaders::VertexColor2D>::set_buffers() {
    _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::VertexColor2D::Position{});
    _mesh.addVertexBuffer(_mesh.color_buffer, 0, Shaders::VertexColor2D::Color4{});
}

template<>
void DrawableBase<Shaders::Flat3D>::gui(const std::string &name_) {
    std::string name = name_;
    if (name.empty()) {
        name = "Flat Shader";
    }
    if (ImGui::TreeNode(name.c_str())) {
        ImGui::Checkbox("Visible", &visible);
        if (!bool(shader().flags() & Shaders::Flat3D::Flag::VertexColor)) {
            ImGui::ColorEdit4("Color", data().color.data());
        }
        ImGui::TreePop();
    }
}
template<>
void DrawableBase<Shaders::VertexColor3D>::gui(const std::string &name_) {
    std::string name = name_;
    if (name.empty()) {
        name = "Vertex Color Shader";
    }
    if (ImGui::TreeNode(name.c_str())) {
        ImGui::Checkbox("Visible", &visible);
        ImGui::TreePop();
    }
}
template<>
void DrawableBase<Shaders::Phong>::gui(const std::string &name_) {
    std::string name = name_;
    if (name.empty()) {
        name = "Phong Shader";
    }
    if (ImGui::TreeNode(name.c_str())) {
        ImGui::Checkbox("Visible", &visible);
        ImGui::ColorEdit4("Ambient Color", data().ambient_color.data());
        if (shader().flags() & Magnum::Shaders::Phong::Flag::VertexColor) {
            ImGui::Text("Vertex Color Mode Specifies Diffuse");
        } else {
            ImGui::ColorEdit4("Diffuse Color", data().diffuse_color.data());
        }
        ImGui::ColorEdit4("Specular Color", data().specular_color.data());

        ImGui::SliderFloat("Shininess", &data().shininess, 0., 100.);
        {
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("Lights", tab_bar_flags)) {
                for (size_t i = 0; i < data().light_positions.size(); ++i) {
                    std::stringstream ss;
                    ss << i;
                    if (ImGui::BeginTabItem(ss.str().c_str())) {
                        ImGui::SliderFloat3("Light Position",
                                            data().light_positions[i].data(),
                                            -100.,
                                            100.);
                        ImGui::ColorEdit4("Light Color",
                                          data().light_colors[i].data());
                        ImGui::EndTabItem();
                    }
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::TreePop();
    }
    // if(shader().flags() & Shaders::Phong::Flag::AlphaMask) {
    //    shader().setAlphaMask(data().alpha_mask);
    //}
}
template<>
void DrawableBase<Shaders::MeshVisualizer3D>::gui(const std::string &name_) {
    std::string name = name_;
    if (name.empty()) {
        name = "Mesh Visualizer Shader";
    }
    if (ImGui::TreeNode(name.c_str())) {
        ImGui::Checkbox("Visible", &visible);
        ImGui::ColorEdit4("Color", data().color.data());
        ImGui::ColorEdit4("Wireframe Color", data().wireframe_color.data());
        ImGui::SliderFloat("Wireframe Width", &data().wireframe_width, 0., 10.);
        ImGui::SliderFloat("Smoothness", &data().smoothness, 0., 10.);
        ImGui::TreePop();
    }
}
template<>
void MeshDrawable<Shaders::Flat3D>::set_buffers() {
    _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::Flat3D::Position{});
    shader().setColor(data().color);
    if (shader().flags() & Shaders::Flat3D::Flag::AlphaMask) {
        shader().setAlphaMask(data().alpha_mask);
    }
}
template<>
void MeshDrawable<Shaders::VertexColor3D>::set_buffers() {
    _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::VertexColor3D::Position{});
    _mesh.addVertexBuffer(_mesh.color_buffer, 0, Shaders::VertexColor3D::Color4{});
}
template<>
void MeshDrawable<Shaders::Phong>::set_buffers() {
    _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::Phong::Position{});
    _mesh.addVertexBuffer(_mesh.normal_buffer, 0, Shaders::Phong::Normal{});
    shader()
      .setShininess(data().shininess)
      .setLightPositions(data().light_positions)
      .setLightColors(data().light_colors);
    if (shader().flags() & Shaders::Phong::Flag::AlphaMask) {
        shader().setAlphaMask(data().alpha_mask);
    }
    if (shader().flags() & Shaders::Phong::Flag::VertexColor) {
        _mesh.addVertexBuffer(_mesh.color_buffer, 0, Shaders::Phong::Color4{});
        shader()
          .setAmbientColor(data().ambient_color)
          .setSpecularColor(data().specular_color);
    } else {
        shader()
          .setDiffuseColor(data().diffuse_color)
          .setAmbientColor(data().ambient_color)
          .setSpecularColor(data().specular_color);
    }
}
template<>
void MeshDrawable<Shaders::MeshVisualizer3D>::set_buffers() {
    _mesh.addVertexBuffer(_mesh.vertex_buffer, 0, Shaders::MeshVisualizer3D::Position{});
    shader().setColor(data().color);
    if (shader().flags() & Magnum::Shaders::MeshVisualizer3D::Flag::Wireframe) {
        shader()
          .setWireframeColor(data().wireframe_color)
          .setWireframeWidth(data().wireframe_width)
          .setSmoothness(data().smoothness);
    }

    if (data().viewport_size) {
        shader().setViewportSize(*data().viewport_size);
    } else {
        shader().setViewportSize(
          Magnum::Vector2{ GL::defaultFramebuffer.viewport().size() });
    }
}
template<>
void DrawableBase<Shaders::Phong>::set_matrices(
  const Matrix4 &transformationMatrix,
  SceneGraph::Camera3D &camera) {
    shader()
      .setTransformationMatrix(transformationMatrix)
      .setNormalMatrix(transformationMatrix.rotationScaling())
      .setProjectionMatrix(camera.projectionMatrix());
}

template<>
void DrawableBase<Shaders::MeshVisualizer3D>::set_matrices(
  const Matrix4 &transformationMatrix,
  SceneGraph::Camera3D &camera) {
    shader()
      .setTransformationMatrix(transformationMatrix)
      .setProjectionMatrix(camera.projectionMatrix());
}
template<>
bool MeshDrawable<Shaders::VertexColor2D>::lint_buffers() {
    bool ret = lint_vertex();
    if (edge_primitive) {
        ret &= lint_edge();
    }
    if (triangle_primitive) {
        ret &= lint_triangle();
    }
    ret &= lint_color();
    return ret;
}
template<>
bool MeshDrawable<Shaders::VertexColor3D>::lint_buffers() {
    bool ret = lint_vertex();
    if (edge_primitive) {
        ret &= lint_edge();
    }
    if (triangle_primitive) {
        ret &= lint_triangle();
    }
    ret &= lint_color();
    return ret;
}
template<>
bool MeshDrawable<Shaders::Phong>::lint_buffers() {
    bool ret = lint_vertex();
    if (edge_primitive) {
        ret &= lint_edge();
    }
    if (triangle_primitive) {
        ret &= lint_triangle();
    }
    ret &= lint_normal();
    return ret;
}
}// namespace mtao::opengl
