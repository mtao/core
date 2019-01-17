#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H


#include "mtao/opengl/renderers/renderer.h"
#include "mtao/opengl/shader.h"
#include "mtao/opengl/vao.h"
#include "mtao/opengl/bo.h"
#include "mtao/util.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#include <Eigen/Dense>
#pragma GCC diagnostic pop
#include <memory>
namespace mtao { namespace opengl { namespace renderers {


    struct MeshRenderBuffers {
        std::unique_ptr<IBO> faces;
        std::unique_ptr<IBO> edges;
        std::unique_ptr<VBO> vertices;
        std::unique_ptr<BO> vectors;
        std::unique_ptr<BO> normals;
        std::unique_ptr<BO> colors;
    };

class MeshRenderer: public Renderer {
    public:
        using MatrixXgf = Eigen::Matrix<GLfloat, Eigen::Dynamic,Eigen::Dynamic>;
        using MatrixXui = Eigen::Matrix<GLuint, Eigen::Dynamic,Eigen::Dynamic>;
        using MatrixXgfRef = Eigen::Ref<MatrixXgf>;
        using MatrixXuiRef = Eigen::Ref<MatrixXui>;
        using MatrixXgfCRef = Eigen::Ref<const MatrixXgf>;
        using MatrixXuiCRef = Eigen::Ref<const MatrixXui>;

        enum class FaceStyle: int { Disabled = 0, Flat, Color, Phong };
        enum class EdgeStyle: int { Disabled= 0, BaryEdge, Mesh, Color};
        enum class VertexStyle: int { Disabled= 0, Flat, Color};

        MeshRenderer(int dim);
        ~MeshRenderer();

        void render() const override;
        void render(const MeshRenderBuffers& buffs) const;
        void render_points(const MeshRenderBuffers& buffs, VertexStyle style = VertexStyle::Disabled) const;
        void render_edges(const MeshRenderBuffers& buffs, EdgeStyle style = EdgeStyle::Disabled) const;
        void render_faces(const MeshRenderBuffers& buffs, FaceStyle style = FaceStyle::Disabled) const;
        void render_vfield(const MeshRenderBuffers& buffs) const;
        void render_points() const;
        void render_edges() const;
        void render_faces() const;
        void render_vfield() const;
        void drawEdges(const MeshRenderBuffers& buffs) const;
        void drawFaces(const MeshRenderBuffers& buffs) const;



        void imgui_interface() override;
        std::list<ShaderProgram*> mvp_programs() const override ;
        void setMesh(const MatrixXgfCRef& V, const MatrixXuiCRef& F, bool normalize = false);
        void setMesh(const MatrixXgfCRef& V, const MatrixXuiCRef& F, const MatrixXgfCRef& N, bool normalize=false);
        void setVertices(const MatrixXgfCRef& V, bool normalize = false);
        void setVField(const MatrixXgfCRef& V);
        void setFaces(const MatrixXgfCRef& V, bool normalize = false);
        void setEdges(const MatrixXgfCRef& V, bool normalize = false);
        void setFaces(const MatrixXuiCRef& F);
        void setEdges(const MatrixXuiCRef& E);
        void setColor(const MatrixXgfCRef& C);
        void setNormals(const MatrixXgfCRef& N);
        void setEdgesFromFaces(const MatrixXuiCRef& F);
        void setMeanEdgeLength(const MatrixXgfCRef& V, const MatrixXuiCRef& F, bool normalize=false);

        //Area weighted normal
        MatrixXgf computeNormals(const MatrixXgfCRef& V, const MatrixXuiCRef& F);


        inline bool shaders_enabled() const { return s_shaders_enabled[m_dim-2]; }
        inline bool& shaders_enabled() { return s_shaders_enabled[m_dim-2]; }
        inline std::unique_ptr<ShaderProgram>&  flat_program() {return s_flat_program[m_dim-2];}
        inline const std::unique_ptr<ShaderProgram>&  flat_program() const {return s_flat_program[m_dim-2];}
        inline std::unique_ptr<ShaderProgram>&  vert_color_program() {return s_vert_color_program[m_dim-2];}
        inline const std::unique_ptr<ShaderProgram>&  vert_color_program() const {return s_vert_color_program[m_dim-2];}
        inline std::unique_ptr<ShaderProgram>&  phong_program() {return s_phong_program[m_dim-2];}
        inline const std::unique_ptr<ShaderProgram>&  phong_program() const {return s_phong_program[m_dim-2];}
        inline std::unique_ptr<ShaderProgram>&  baryedge_program() {return s_baryedge_program[m_dim-2];}
        inline const std::unique_ptr<ShaderProgram>&  baryedge_program() const {return s_baryedge_program[m_dim-2];}
        inline std::unique_ptr<ShaderProgram>&  vector_field_program() {return s_vector_field_program[m_dim-2];}
        inline const std::unique_ptr<ShaderProgram>&  vector_field_program() const {return s_vector_field_program[m_dim-2];}


        inline FaceStyle get_face_style() {
            return m_face_style;
        }
        inline EdgeStyle get_edge_style() {
            return m_edge_type;
        }
        inline VertexStyle get_vertex_style() {
            return m_vertex_type;
        }

        inline void set_face_style(FaceStyle style=FaceStyle::Disabled) {
            m_face_style = style;
        }
        inline void set_edge_style(EdgeStyle style=EdgeStyle::Disabled) {
            m_edge_type = style;
        }
        inline void set_vertex_style(VertexStyle style=VertexStyle::Disabled) {
            m_vertex_type = style;
        }
        inline void show_vector_field(bool yes=  true) {
            m_show_vector_field = yes;
        }

        void unset_all();
        void hide_all() { unset_all(); }

        inline void setBuffers(const std::shared_ptr<MeshRenderBuffers>& buf) { m_buffers = buf; }




        MTAO_ACCESSORS(glm::vec4,face_color,m_face_color)
        MTAO_ACCESSORS(glm::vec4,edge_color,m_edge_color)
        MTAO_ACCESSORS(glm::vec4,vertex_color,m_vertex_color)
        MTAO_ACCESSORS(float,edge_threshold,m_edge_threshold)
        MTAO_ACCESSORS(float,line_width,m_line_width)
        MTAO_ACCESSORS(float,point_size,m_point_size)

        MTAO_ACCESSORS(glm::vec3,light_pos,m_light_pos )
        MTAO_ACCESSORS(glm::vec4,ambientMat,m_ambientMat )
        MTAO_ACCESSORS(glm::vec4,diffuseMat,m_diffuseMat )
        MTAO_ACCESSORS(glm::vec4,spectularMat,m_specularMat )
        MTAO_ACCESSORS(float,specularExpMat,m_specularExpMat )

        MTAO_ACCESSORS(float,vector_scale,m_vector_scale )
        MTAO_ACCESSORS(float,vector_color_scale,m_vector_color_scale )
        MTAO_ACCESSORS(glm::vec4,vector_tip_color,m_vector_tip_color )
        MTAO_ACCESSORS(glm::vec4,vector_base_color,m_vector_base_color )



        MeshRenderBuffers* buffers() { return m_buffers.get(); }
        const MeshRenderBuffers* buffers() const { return m_buffers.get(); }
    private:
        void loadShaders(int dim);
        void update_edge_threshold();
        void update_phong_shading();
        void update_vertex_scale(const MatrixXgfCRef& V);
        std::array<float,2> get_line_width_range() const;
        std::array<float,2> get_point_size_range() const;

        static bool s_shaders_enabled[2];

        static std::unique_ptr<ShaderProgram> s_flat_program[2];

        static std::unique_ptr<ShaderProgram> s_vert_color_program[2];

        static std::unique_ptr<ShaderProgram> s_phong_program[2];

        static std::unique_ptr<ShaderProgram> s_baryedge_program[2];

        static std::unique_ptr<ShaderProgram> s_vector_field_program[2];


        std::shared_ptr<MeshRenderBuffers> m_buffers;

        FaceStyle m_face_style = FaceStyle::Phong;
        EdgeStyle m_edge_type = EdgeStyle::Disabled;
        VertexStyle m_vertex_type = VertexStyle::Flat;
        //control whetehr edge/face  should use aan index buffer (drawElements) or the vertices themselves (drawArrays)
        bool m_edge_draw_elements = true;
        bool m_face_draw_elements = true;

        bool m_show_vector_field = false;
        bool m_use_line_smooth = false;//antialiasing
        bool m_use_polygon_smooth = false;//antialiasing

        int m_dim=2;

        float m_vertex_scale = 1.0;

        glm::vec4 m_face_color = glm::vec4(1,0,0,1);
        glm::vec4 m_edge_color = glm::vec4(0,1,0,1);
        glm::vec4 m_vertex_color = glm::vec4(0,0,1,1);
        float m_edge_threshold = 0.001;
        float m_line_width = 1;
        float m_point_size = 1;

        //phong shading program
        glm::vec3 m_light_pos = glm::vec3(0,10,0);
        glm::vec4 m_ambientMat = glm::vec4(.1f,.1f,.1f,1.f);
        glm::vec4 m_diffuseMat = glm::vec4(.7f,.7f,.7f,1.f);
        glm::vec4 m_specularMat = glm::vec4(.99f,.99f,.99f,1.f);
        float m_specularExpMat = 20.0f;

        //Vector field program
        float m_vector_scale = 0.1;
        float m_vector_color_scale = 0.1;
        glm::vec4 m_vector_tip_color = glm::vec4(1,0,0,1);
        glm::vec4 m_vector_base_color = glm::vec4(0,0,0,1);


};
}}}


#endif//MESH_RENDERER_H
