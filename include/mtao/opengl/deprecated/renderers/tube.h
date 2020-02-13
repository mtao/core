#pragma once
#include "mesh.h"
namespace mtao { namespace opengl { namespace renderers {
    class TubeRenderer: public MeshRenderer {

        public:
    static std::unique_ptr<ShaderProgram> s_tube_program;
    static bool s_shader_enabled;
    using MatrixXgf = MeshRenderer::MatrixXgf;
    using MatrixXui = MeshRenderer::MatrixXui;
    using MatrixXgfCRef = MeshRenderer::MatrixXgfCRef;
    using MatrixXuiCRef = MeshRenderer::MatrixXuiCRef;

    TubeRenderer();
    ~TubeRenderer();

    void render() const;
    void addEdges(const MatrixXgfCRef& V);
    void imgui_interface(const std::string& name); 
        private:
    bool m_visible = true;
    MatrixXgf m_V;
    MatrixXgf m_T;
    MatrixXgf m_N;
    MatrixXui m_E;
    float m_radius = .1;
    float m_mean_edge_length = .1;
    };
}}}
