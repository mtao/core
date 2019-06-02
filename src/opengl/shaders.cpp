/*
#include "mtao/opengl/shaders.h"
#include <Magnum/GL/DefaultFramebuffer.h>
using namespace Magnum;
namespace mtao::opengl::shaders {
    void ShaderPack2::activate_flat() {
        activate_flat(flat_flags);
    }
    void ShaderPack2::activate_flat(const Shaders::Flat2D::Flags& flags) {
        emplace<Shaders::Flat2D>(flags);

    }

    void ShaderPack2::activate_vertex_color() {
        emplace<Shaders::VertexColor2D>();
    }

    void ShaderPack3::activate_flat(const Shaders::Flat3D::Flags& flags) {
        emplace<Shaders::Flat3D>(flags);
    }
    void ShaderPack3::activate_phong(const Shaders::Phong::Flags& flags) {
        emplace<Shaders::Phong>(flags);
    }
    void ShaderPack3::activate_mesh_visualizer(const Shaders::MeshVisualizer::Flags& flags) {
        emplace<Shaders::MeshVisualizer>(flags);
    }
    void ShaderPack3::activate_flat(){
        activate_flat(flat_flags);
    }
    void ShaderPack3::activate_vertex_color(){
        emplace<Shaders::VertexColor3D>();
    }
    void ShaderPack3::activate_phong(){
        activate_phong(phong_flags);
    }
    void ShaderPack3::activate_mesh_visualizer(){
        activate_mesh_visualizer(mesh_visualizer_flags);
    }



    void ShaderPack2::draw(objects::Mesh& mesh) {
        std::visit([&](auto&& shader) {
                using T = std::decay_t<decltype(shader)>;
                mesh.addVertexBuffer(mesh.vertex_buffer, 0, typename T::Position{});

                if constexpr(std::is_same_v<T,Shaders::Flat2D>) {
                shader.setColor(data.flat_color).setAlphaMask(data.flat_alpha_mask);
                } else if constexpr(std::is_same_v<T,Shaders::VertexColor2D>) {
                }
                mesh.draw(shader);
                },base());
    }
    void ShaderPack3::draw(objects::Mesh& mesh) {
        std::visit([&](auto&& shader) {
                using T = std::decay_t<decltype(shader)>;
                mesh.addVertexBuffer(mesh.vertex_buffer, 0, typename T::Position{});
                if constexpr(std::is_same_v<T,Shaders::Flat3D>) {
                } else if constexpr(std::is_same_v<T,Shaders::VertexColor3D>) {
                } else if constexpr(std::is_same_v<T,Shaders::Phong>) {
                } else if constexpr(std::is_same_v<T,Shaders::MeshVisualizer>) {
                }
                mesh.draw(shader);
                },base());
    }
}
*/
