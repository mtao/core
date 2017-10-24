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

        MeshRenderer(int dim);

        void render() const override;
        void set_mvp(const glm::mat4& mvp);
        void set_face_color(const glm::vec3& col);
        void set_edge_color(const glm::vec3& col);
        void set_edge_threshold(float thresh);
        void setMesh(const MatrixXgf& V, const MatrixXui& F);
        void setEdges(const MatrixXui& E);
        void setEdgesFromFaces(const MatrixXui& E);
    private:
        void loadShaders(int dim);

        std::unique_ptr<ShaderProgram> m_flat_program;
        std::unique_ptr<ShaderProgram> m_baryedge_program;

        std::unique_ptr<IBO> m_index_buffer;
        std::unique_ptr<IBO> m_edge_index_buffer;
        std::unique_ptr<BO> m_vertex_buffer;


        bool m_use_baryedge = true;
        bool m_phong_faces = false;
        bool m_draw_edges = true;

        glm::vec3 m_face_color = glm::vec3(1,0,0);
        glm::vec3 m_edge_color = glm::vec3(0,1,0);



};
}}}


#endif//MESH_RENDERER_H
