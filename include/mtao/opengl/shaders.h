#ifndef SHADERS_H
#define SHADERS_H
#pragma once
#include <Magnum/Magnum.h>
#include "mtao/opengl/objects/mesh.h"
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/MeshVisualizer.h>
#include <Magnum/Shaders/Flat.h>
#include <Corrade/Containers/Array.h>
#include <variant>
#include <optional>
namespace mtao::opengl {


    using namespace Magnum;
    using namespace Math::Literals;
    /*
    class ShaderPack2: public std::variant<Shaders::Flat2D,Shaders::VertexColor2D> {

        using Base = std::variant<Shaders::Flat2D,Shaders::VertexColor2D> ;
        Base& base() { return *static_cast<Base*>(this); }
        const Base& base() const { return *static_cast<const Base*>(this); }
        void draw(objects::Mesh& mesh);
        void activate_flat();
        void activate_flat(const Shaders::Flat2D::Flags&);

        void activate_vertex_color();

    };
    class ShaderPack3: public std::variant<Shaders::Flat3D,Shaders::VertexColor3D,Shaders::Phong,Shaders::MeshVisualizer> {
        using Base = std::variant<Shaders::Flat3D,Shaders::VertexColor3D,Shaders::Phong,Shaders::MeshVisualizer>;
        Base& base() { return *static_cast<Base*>(this); }
        const Base& base() const { return *static_cast<const Base*>(this); }
        void draw(objects::Mesh& mesh);
        void activate_flat();
        void activate_flat(const Shaders::Flat3D::Flags&);

        void activate_vertex_color();

        void activate_phong();
        void activate_phong(const Shaders::Phong::Flags&);

        void activate_mesh_visualizer();
        void activate_mesh_visualizer(const Shaders::MeshVisualizer::Flags&);
    };

    struct ShaderData3 {
    };
    */
    template <typename ShaderType>
        struct ShaderData {};

    template <>
    struct ShaderData<Shaders::Flat3D>{
        Shaders::Flat3D::Flags flags = {};
        Color4 color = 0x2f83cc_rgbf;
        Float alpha_mask = .5f;
    };
    template <>
    struct ShaderData<Shaders::Flat2D>{
        Shaders::Flat3D::Flags flags = {};
        Color4 color = 0x2f83cc_rgbf;
        Float alpha_mask = .5f;
    };

    template <>
    struct ShaderData<Shaders::Phong>{
        Shaders::Phong::Flags flags = {};
        Float alpha_mask = .5f;
        Color4 ambient_color = 0x2f83cc_rgbf;
        Color4 diffuse_color = 0x2f83cc_rgbf;
        Color4 specular_color = 0x2f83cc_rgbf;
        Float shininess = 200.f;

        Containers::Array<Magnum::Vector3> light_positions = Containers::Array<Magnum::Vector3> {Containers::InPlaceInit,{Magnum::Vector3{5.0f,5.0f,7.0f}}};
        Containers::Array<Color4> light_colors = Containers::Array<Color4> {Containers::InPlaceInit,{0xffffff_rgbf}};
    };

    template <>
    struct ShaderData<Shaders::MeshVisualizer3D>{
        Shaders::MeshVisualizer3D::Flags flags = {};
        Color4 color = 0x2f83cc_rgbf;
        Color4 wireframe_color = 0xdcdcdc_rgbf;
        Float wireframe_width = 1.f;
        Float smoothness = 1.f;
        std::optional<Magnum::Vector2> viewport_size;
    };



}
#endif//SHADERS_H
