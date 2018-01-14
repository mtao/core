#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H


#include "mtao/opengl/renderers/renderer.h"
#include "mtao/opengl/shader.h"
#include "mtao/opengl/vao.h"
#include "mtao/opengl/bo.h"
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

        enum class FaceStyle: int { Disabled = 0, Flat, Color, Phong };
        enum class EdgeType: int { Disabled= 0, BaryEdge, Mesh};
        enum class VertexType: int { Disabled= 0, Flat, Color};

        MeshRenderer(int dim);

        void render() const override;
        void render(const MeshRenderBuffers& buffs) const;
        void render_points(const MeshRenderBuffers& buffs, VertexType style = VertexType::Disabled) const;
        void render_edges(const MeshRenderBuffers& buffs, EdgeType style = EdgeType::Disabled) const;
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
        void setMesh(const MatrixXgf& V, const MatrixXui& F, bool normalize = false);
        void setMesh(const MatrixXgf& V, const MatrixXui& F, const MatrixXgf& N, bool normalize=false);
        void setVertices(const MatrixXgf& V, bool normalize = false);
        void setVField(const MatrixXgf& V);
        void setFaces(const MatrixXgf& V, bool normalize = false);
        void setEdges(const MatrixXgf& V, bool normalize = false);
        void setFaces(const MatrixXui& F);
        void setEdges(const MatrixXui& E);
        void setColor(const MatrixXgf& C);
        void setNormals(const MatrixXgf& N);
        void setEdgesFromFaces(const MatrixXui& F);
        void setMeanEdgeLength(const MatrixXgf& V, const MatrixXui& F, bool normalize=false);

        //Area weighted normal
        MatrixXgf computeNormals(const MatrixXgf& V, const MatrixXui& F);


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
        inline EdgeType get_edge_style() {
            return m_edge_type;
        }
        inline VertexType get_vertex_style() {
            return m_vertex_type;
        }

        inline void set_face_style(FaceStyle style=FaceStyle::Disabled) {
            m_face_style = style;
        }
        inline void set_edge_style(EdgeType style=EdgeType::Disabled) {
            m_edge_type = style;
        }
        inline void set_vertex_type(VertexType style=VertexType::Disabled) {
            m_vertex_type = style;
        }
        inline void show_vector_field(bool yes=  true) {
            m_show_vector_field = yes;
        }

        void unset_all();

        inline void setBuffers(const std::shared_ptr<MeshRenderBuffers>& buf) { m_buffers = buf; }

        MeshRenderBuffers* buffers() { return m_buffers.get(); }
        const MeshRenderBuffers* buffers() const { return m_buffers.get(); }
    private:
        void loadShaders(int dim);
        void update_edge_threshold();
        void update_phong_shading();
        void update_vertex_scale(const MatrixXgf& V);

        static bool s_shaders_enabled[2];

        static std::unique_ptr<ShaderProgram> s_flat_program[2];

        static std::unique_ptr<ShaderProgram> s_vert_color_program[2];

        static std::unique_ptr<ShaderProgram> s_phong_program[2];

        static std::unique_ptr<ShaderProgram> s_baryedge_program[2];

        static std::unique_ptr<ShaderProgram> s_vector_field_program[2];


        std::shared_ptr<MeshRenderBuffers> m_buffers;

        FaceStyle m_face_style = FaceStyle::Phong;
        EdgeType m_edge_type = EdgeType::Disabled;
        VertexType m_vertex_type = VertexType::Flat;
        //control whetehr edge/face  should use aan index buffer (drawElements) or the vertices themselves (drawArrays)
        bool m_edge_draw_elements = true;
        bool m_face_draw_elements = true;

        bool m_show_vector_field = false;

        int m_dim=2;

        float m_vertex_scale = 1.0;

        glm::vec3 m_face_color = glm::vec3(1,0,0);
        glm::vec3 m_edge_color = glm::vec3(0,1,0);
        glm::vec3 m_vertex_color = glm::vec3(0,0,1);
        float m_edge_threshold = 0.001;

        //phong shading program
        glm::vec3 m_light_pos = glm::vec3(0,10,0);
        glm::vec4 m_ambientMat = glm::vec4(.1f,.1f,.1f,1.f);
        glm::vec4 m_diffuseMat = glm::vec4(.7f,.7f,.7f,1.f);
        glm::vec4 m_specularMat = glm::vec4(.99f,.99f,.99f,1.f);
        float m_specularExpMat = 20.0f;

        //Vector field program
        float m_vector_scale = 0.1;
        float m_vector_color_scale = 0.1;
        glm::vec3 m_vector_tip_color = glm::vec3(1,0,0);
        glm::vec3 m_vector_base_color = glm::vec3(0,0,0);


};
}}}


#endif//MESH_RENDERER_H
