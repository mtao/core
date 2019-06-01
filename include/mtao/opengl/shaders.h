#ifndef SHADERS_H
#define SHADERS_H
#pragma once
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/MeshVisualizer.h>
#include <Magnum/Shaders/Flat.h>
namespace mtao::opengl {

    class ShaderPack2: public std::variant<Shaders::Flat2D,Shaders::VertexColor2D> {
        std::optional<Shaders::Flat2D> flat;
        std::optional<Shaders::VertexColor2D> vertex_color;
        void activate_flat();
        void activate_flat(const Shaders::Flat2D::Flags&);
        void activate_vertex_color();

        Shaders::Flat2D::Flags flat_flags;
        Shaders::VertexColor::Flags flat_flags;
    };
    class ShaderPack3: public std::variant<Shaders::Flat3D,Shaders::VertexColor3D,Shaders::Phong,Shaders::MeshVisualizer> shader> {
        void activate_flat(const Shaders::Flat3D::Flags&);
        void activate_vertex_color();
        void activate_phong(const Shaders::Phong::Flags&);
        void activate_mesh_visualizer(const Shaders::MeshVisualizer::Flags&);
        void activate_flat();
        void activate_phong();
        void activate_mesh_visualizer();
    };
}
#endif//SHADERS_H
