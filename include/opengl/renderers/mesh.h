#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H


#include "renderer.h"
#include "opengl/shader.h"
#include "opengl/vao.h"
#include "opengl/bo.h"
#include <Eigen/Dense>
#include <memory>
namespace mtao { namespace opengl { namespace renderers {

class MeshRenderer: public Renderer {
    public:
        using MatrixXgf = Eigen::Matrix<GLfloat, Eigen::Dynamic,Eigen::Dynamic>;
        using MatrixXui = Eigen::Matrix<GLuint, Eigen::Dynamic,Eigen::Dynamic>;

        enum class FaceStyle: int { Disabled = 0, Flat, Color, Phong };
        enum class EdgeType: int { Disabled= 0, BaryEdge, Mesh};

        MeshRenderer(int dim);

        void render() const override;
        void imgui_interface() override;
        std::list<ShaderProgram*> mvp_programs() const override ;
        void setMesh(const MatrixXgf& V, const MatrixXui& F, bool normalize = false);
        void setMesh(const MatrixXgf& V, const MatrixXui& F, const MatrixXgf& N, bool normalize=false);
        void setColor(const MatrixXgf& C);
        void setEdges(const MatrixXui& E);
        void setEdgesFromFaces(const MatrixXui& E);

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



        inline void set_face_style(FaceStyle style=FaceStyle::Disabled) {
            m_face_style = style;
        }
    private:
        void loadShaders(int dim);
        void update_edge_threshold();
        void update_phong_shading();

        static bool s_shaders_enabled[2];

        static std::unique_ptr<ShaderProgram> s_flat_program[2];

        static std::unique_ptr<ShaderProgram> s_vert_color_program[2];

        static std::unique_ptr<ShaderProgram> s_phong_program[2];

        static std::unique_ptr<ShaderProgram> s_baryedge_program[2];


        std::unique_ptr<IBO> m_index_buffer;
        std::unique_ptr<IBO> m_edge_index_buffer;
        std::unique_ptr<VBO> m_vertex_buffer;
        std::unique_ptr<BO> m_normal_buffer;
        std::unique_ptr<BO> m_color_buffer;

        FaceStyle m_face_style = FaceStyle::Phong;
        EdgeType m_edge_type = EdgeType::Disabled;

        int m_dim=2;
        bool m_draw_points = false;


        glm::vec3 m_face_color = glm::vec3(1,0,0);
        glm::vec3 m_edge_color = glm::vec3(0,1,0);
        glm::vec3 m_vertex_color = glm::vec3(0,0,1);
        float m_edge_threshold = 0.001;

        glm::vec3 m_light_pos = glm::vec3(0,10,0);
        glm::vec4 m_ambientMat = glm::vec4(.1f,.1f,.1f,1.f);
        glm::vec4 m_diffuseMat = glm::vec4(.7f,.7f,.7f,1.f);
        glm::vec4 m_specularMat = glm::vec4(.99f,.99f,.99f,1.f);
        float m_specularExpMat = 20.0f;



};
}}}


#endif//MESH_RENDERER_H
