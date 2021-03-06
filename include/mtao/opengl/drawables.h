#pragma once
#include <Magnum/Magnum.h>
#include "mtao/opengl/objects/mesh.h"
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/MeshVisualizer.h>
#include <Magnum/Shaders/Generic.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/Buffer.h>
#include "mtao/opengl/shaders.h"


namespace mtao::opengl {
    namespace internal {
        template <int Dim>
        constexpr int ShaderDim(const Shaders::Generic<Dim>*) {
            return Dim;
        }
    template <typename ShaderType>
    constexpr int ShaderDim() {
        return ShaderType::Position::Type::Size;
        //return internal::ShaderDim(std::declval<ShaderType*>());
    }
    //I know that this is more complicated because i did a typedef elsewhere (could just template it all) but this works for now
    template <typename ShaderType>
    using DrawableType = std::conditional_t<ShaderDim<ShaderType>() == 2,
          SceneGraph::Drawable2D, SceneGraph::Drawable3D>;
    template <typename ShaderType>
    using DrawableGroupType = std::conditional_t<ShaderDim<ShaderType>() == 2,
          SceneGraph::DrawableGroup2D, SceneGraph::DrawableGroup3D>;
    template <typename ShaderType>
    using CameraType = std::conditional_t<ShaderDim<ShaderType>() == 2,
          SceneGraph::Camera2D, SceneGraph::Camera3D>;
    template <typename ShaderType>
    using TransformMatrixType = std::conditional_t<ShaderDim<ShaderType>() == 2,
          Matrix3,Matrix4>;

    }


    template <typename ShaderType>
        class Drawable: public internal::DrawableType<ShaderType> {
        public:
            constexpr static int D = internal::ShaderDim<ShaderType>();
            using Camera = internal::CameraType<ShaderType>;
            using DrawableGroup = internal::DrawableGroupType<ShaderType>;
            using TransMat = internal::TransformMatrixType<ShaderType>;
            using MeshType = objects::Mesh<D>;
            explicit Drawable(MeshType& mesh, ShaderType& shader, DrawableGroup& group): internal::DrawableType<ShaderType>{mesh, &group},  _mesh(mesh),_shader(shader) {}
            void gui(const std::string& name = "");

            void deactivate() {
                activate_points({});
                activate_edges({});
                activate_triangles({});
            }
            void activate_points(const std::optional<GL::MeshPrimitive>& p = GL::MeshPrimitive::Points) {
                point_primitive = p;
            }
            void activate_edges(const std::optional<GL::MeshPrimitive>& p = GL::MeshPrimitive::Lines) {
                edge_primitive = p;
            }
            void activate_triangles(const std::optional<GL::MeshPrimitive>& p = GL::MeshPrimitive::Triangles) {
                triangle_primitive = p;
            }
            ShaderData<ShaderType>&  data() { return _data; } 

            void set_visibility(bool val) { visible = val; }
            bool is_visible() const { return visible; }

            bool lint_buffers(){
                bool ret = lint_vertex();
                if(edge_primitive) { ret &= lint_edge(); }
                if(triangle_primitive) { ret &= lint_triangle(); }
                return ret;
            }
        private:
            bool visible = true;
            std::optional<GL::MeshPrimitive> triangle_primitive = GL::MeshPrimitive::Triangles;
            std::optional<GL::MeshPrimitive> edge_primitive;// = GL::MeshPrimitive::Lines;
            std::optional<GL::MeshPrimitive> point_primitive;// = GL::MeshPrimitive::Lines;
            MeshType& _mesh;
            ShaderType& _shader;
            ShaderData<ShaderType> _data;

            void set_matrices(const TransMat& transformationMatrix, Camera& camera)  {
                _shader.setTransformationProjectionMatrix(camera.projectionMatrix()*transformationMatrix);
            }
            void draw(const TransMat& transformationMatrix, Camera& camera) override {
                if(!visible) return;

                set_buffers();
                set_matrices(transformationMatrix, camera);
                if(triangle_primitive) {

                    _mesh.setCount(_mesh.triangle_Count);
                    _mesh.setIndexBuffer(_mesh.triangle_index_buffer,0,_mesh.triangle_indexType,_mesh.triangle_indexStart,_mesh.triangle_indexEnd);
                    _mesh.setPrimitive(*triangle_primitive);
                    _mesh.draw(_shader);
                }
                if(edge_primitive) {
                    _mesh.setCount(_mesh.edge_Count);
                    _mesh.setIndexBuffer(_mesh.edge_index_buffer,0,_mesh.edge_indexType,_mesh.edge_indexStart,_mesh.edge_indexEnd);
                    _mesh.setPrimitive(*edge_primitive);
                    _mesh.draw(_shader);
                }
                if(point_primitive) {
                    _mesh.setCount(_mesh.vertex_Count);
                    _mesh.setIndexBuffer(_mesh.vertex_index_buffer,0,_mesh.vertex_indexType,_mesh.vertex_indexStart,_mesh.vertex_indexEnd);
                    _mesh.setPrimitive(*point_primitive);
                    _mesh.draw(_shader);
                }
            }
            void set_buffers();
            bool lint_buffer( Magnum::GL::Buffer& b)  { return b.size() > 0; }
            bool lint_vertex()  { return lint_buffer(_mesh.vertex_buffer); }
            bool lint_edge()  { return lint_buffer(_mesh.edge_index_buffer); }
            bool lint_triangle()  { return lint_buffer(_mesh.triangle_index_buffer); }
            bool lint_normal()  { return lint_buffer(_mesh.normal_buffer); }
            bool lint_vfield()  { return lint_buffer(_mesh.vfield_buffer); }
            bool lint_color()  { return lint_buffer(_mesh.color_buffer); }
        };
    template <typename ShaderType, int D,
             typename Camera = internal::CameraType<ShaderType>,
             typename DrawableGroup = internal::DrawableGroupType<ShaderType>,
             typename TransMat = internal::TransformMatrixType<ShaderType>>
                 auto make_drawable(objects::Mesh<D>& mesh, ShaderType& shader, DrawableGroup& group) {
                     return new Drawable<ShaderType>(mesh,shader,group);
                 }
    template <>
    void Drawable<Shaders::Flat2D>::gui(const std::string& name);
    template <>
    void Drawable<Shaders::VertexColor2D>::gui(const std::string& name);
    template <>
    void Drawable<Shaders::Flat2D>::set_buffers();
    template <>
    void Drawable<Shaders::VertexColor2D>::set_buffers();
    template <>
    void Drawable<Shaders::VertexColor2D>::set_buffers();


    template <>
        bool Drawable<Shaders::VertexColor3D>::lint_buffers() ;
    template <>
        bool Drawable<Shaders::VertexColor2D>::lint_buffers() ;
    template <>
        bool Drawable<Shaders::Phong>::lint_buffers() ;


    template <>
    void Drawable<Shaders::Flat3D>::gui(const std::string& name);
    template <>
    void Drawable<Shaders::VertexColor3D>::gui(const std::string& name);
    template <>
    void Drawable<Shaders::Phong>::gui(const std::string& name);
    template <>
    void Drawable<Shaders::MeshVisualizer>::gui(const std::string& name);
    template <>
    void Drawable<Shaders::Flat3D>::set_buffers();
    template <>
    void Drawable<Shaders::VertexColor3D>::set_buffers();
    template <>
    void Drawable<Shaders::Phong>::set_buffers();
    template <>
    void Drawable<Shaders::MeshVisualizer>::set_buffers();
    template <>
    void Drawable<Shaders::Phong>::set_matrices(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) ;

    /*
    class ShaderPackDrawable3: public SceneGraph::Drawable3D {
        public:
            explicit ShaderPack2Drawable(objects::Mesh& mesh, ShaderPack& shader, SceneGraph::DrawableGroup3D& group): SceneGraph::Drawable3D{mesh, &group},  _mesh(mesh),_shader(shader) {}

        private:
            void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

            std::shared_ptr<Shaders::Flat3D> flat_shader;
            std::shared_ptr<Shaders::VertexColor3D> vertex_shader:
            std::shared_ptr<Shaders::Phong> phong_shader:
            std::shared_ptr<Shaders::MeshVisualizer> mesh_visualizer_shader;
            objects::Mesh& _mesh;
    };
    */
};
