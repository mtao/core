#include "mtao/opengl/opengl_loader.hpp"
#include "mtao/opengl/renderers/tube.h"
#include <exception>
#include <stdexcept>
#include <sstream>
#include <array>
#include <list>
#include <imgui.h>
#include <mtao/eigen/stack.h>
#include <glm/gtc/type_ptr.hpp>
#include "mtao/opengl/shaders.h"
#include "mtao/logging/logger.hpp"
#include "mtao/geometry/bounding_box.hpp"
#include "mtao/geometry/mesh/vertex_normals.hpp"
#include "mtao/data_structures/disjoint_set.hpp"
using namespace mtao::logging;

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*_ARR)))

namespace mtao {
namespace opengl {
    namespace renderers {


        bool TubeRenderer::s_shader_enabled = false;
        std::unique_ptr<ShaderProgram> TubeRenderer::s_tube_program;

        TubeRenderer::~TubeRenderer() {}
        TubeRenderer::TubeRenderer() : MeshRenderer(3) {
            if (!s_shader_enabled) {
                s_tube_program = std::make_unique<ShaderProgram>(shaders::tube_shader_program());
                s_shader_enabled = true;
            }
            hide_all();
            line_width() = 5;
        }


        void TubeRenderer::render() const {
            auto &&buffs = *buffers();
            if (m_visible) {
                auto vao_a = vao().enableRAII();
                if (!buffs.vertices) {
                    return;
                }

                auto active = s_tube_program->useRAII();

                if (buffs.vectors && buffs.normals) {
                    auto active = s_tube_program->useRAII();
                    s_tube_program->setUniform("edge_width", m_mean_edge_length);
                    s_tube_program->setUniform("radius", m_radius);
                    auto vpos_active = s_tube_program->getAttrib("vPos").enableRAII();
                    auto vvel_active = s_tube_program->getAttrib("vTan").enableRAII();
                    auto vnorm_active = s_tube_program->getAttrib("vNormal").enableRAII();
                    buffs.edges->drawElements();


                } else {
                    mtao::logging::warn() << "vertex velocities not set, can't render vfield";
                }
            }
        }
        void TubeRenderer::addEdges(const MatrixXgfCRef &V) {
            mtao::ColVectors<GLfloat, 3> T(V.rows(), V.cols());
            MatrixXui E(2, V.cols() - 1);
            using Vert = mtao::Vector<GLfloat, 3>;

            for (int i = 0; i < E.cols(); ++i) {
                E(0, i) = m_V.cols() + i;
                E(1, i) = m_V.cols() + i + 1;
            }
            bool use_first_order = false;

            if (!use_first_order) {
                T.resizeLike(V);
                T.rightCols(V.cols() - 1) = V.rightCols(V.cols() - 1) - V.leftCols(V.cols() - 1);
                T.col(0) = T.col(1);
                T.colwise().normalize();
            } else {
                T.col(0) = V.col(1) - V.col(0);
                for (int i = 1; i < V.cols() - 1; ++i) {
                    T.col(i) = V.col(i + 1) - V.col(i - 1);
                }
                T.col(V.rows() - 1) = V.col(V.cols() - 1) - V.col(V.cols() - 2);
                T.colwise().normalize();
            }


            Vert t = T.col(0);
            Vert u = t.cross(Vert::Unit(0));
            if (u.norm() < 1e-5) {
                u = t.cross(Vert::Unit(1));
            }

            MatrixXgf N(3, T.cols());
            N.col(0) = u;
            for (int i = 1; i < N.cols(); ++i) {
                Vert p = T.col(i - 1).cross(T.col(i));
                Vert axis = p.normalized();
                float s = p.norm();
                float c = T.col(i - 1).dot(T.col(i));

                float theta = std::atan2(s, c);

                auto P = Eigen::AngleAxisf(theta, axis);
                Vert tu = P * T.col(i - 1);
                if ((tu - T.col(i)).norm() > 1e-2) {
                    std::cout << "FRAME COMPUTATION FAILURE!" << std::endl;
                }
                t = P * t;
                u = P * u;
                N.col(i) = u;
            }
            m_V = mtao::eigen::hstack(m_V, V);
            m_E = mtao::eigen::hstack(m_E, E);
            m_N = mtao::eigen::hstack(m_N, N);
            m_T = mtao::eigen::hstack(m_T, T);
            MeshRenderer::setVertices(m_V);
            MeshRenderer::setEdges(m_E);
            MeshRenderer::setNormals(m_N);
            MeshRenderer::setVField(m_T);
        }
        void TubeRenderer::imgui_interface(const std::string &name) {
            ImGui::Checkbox(name.c_str(), &m_visible);
            if (m_visible) {
                if (ImGui::TreeNode((name + " options").c_str())) {

                    MeshRenderer::imgui_interface("Mesh Rendering Interface");
                }
            }
        }
    }// namespace renderers
}// namespace opengl
}// namespace mtao
