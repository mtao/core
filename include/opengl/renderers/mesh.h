#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H


#include "renderer.h"
#include "opengl/shader.h"
#include <memory>
namespace mtao { namespace opengl { namespace renderers {

class MeshRenderer: public Renderer {
    public:

        MeshRenderer(int dim);

        void render() const override;
        void set_mvp(const glm::mat4& mvp);
        void set_face_color(const glm::vec4& col);
        void set_edge_color(const glm::vec4& col);
        void setMesh(const Eigen::MatrixXf& V, const Eigen::MatrixXi& E);
    private:
        void loadShaders(int dim);

        std::unique_ptr<ShaderProgram> m_flat_renderer;
        std::unique_ptr<ShaderProgram> m_baryedge_renderer;

        std::unique_ptr<IBO> m_index_buffer;
        std::unique_ptr<IBO> m_edge_index_buffer;
        std::unique_ptr<BO> m_vertex_buffer;

        glm::vec4 m_face_color = glm::vec4(1,0,0,1);
        glm::vec4 m_edge_color = glm::vec4(0,1,0,1);



};
}}}


#endif//MESH_RENDERER_H
