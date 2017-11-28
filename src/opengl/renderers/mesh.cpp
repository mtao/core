#include "opengl/renderers/mesh.h"
#include <exception>
#include <stdexcept>
#include <sstream>
#include <list>
#include <glad/glad.h>
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include "opengl/shaders.h"
#include "logging/logger.hpp"
#include "geometry/bounding_box.hpp"
using namespace mtao::logging;

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace mtao { namespace opengl { namespace renderers {


    bool MeshRenderer::s_shaders_enabled[2] = {false,false};
    std::unique_ptr<ShaderProgram> MeshRenderer::s_flat_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_phong_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_baryedge_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_vert_color_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_vector_field_program[2];

    MeshRenderer::MeshRenderer(int dim): m_dim(dim) {
        if(dim == 2 || dim == 3) {
            if(!shaders_enabled()) {
                loadShaders(dim);
            }
            update_edge_threshold();
            update_phong_shading();
        } else {
            throw std::invalid_argument("Meshes have to be dim 2 or 3");
        }
        m_buffers = std::make_shared<MeshRenderBuffers>();
    }

    void MeshRenderer::update_vertex_scale(const MatrixXgf& V) {
        auto minPos = V.rowwise().minCoeff();
        auto maxPos = V.rowwise().maxCoeff();
        auto range = maxPos - minPos;

        float r = range.maxCoeff();;
        for(int i = 0; i < range.cols(); ++i) {
            if(range(i) > 1e-5) {
                r = std::min(r,range(i));
            }
        }

    }

    auto MeshRenderer::computeNormals(const MatrixXgf& V, const MatrixXui& F) -> MatrixXgf {
        MatrixXgf N;
        if(m_dim == 2) {
            return N;
        }
        N.resizeLike(V);
        N.setZero();
        //Eigen::VectorXf areas(V.cols());
        //areas.setZero();
        for(int i = 0; i < F.cols(); ++i) {
            auto f = F.col(i);
            auto a = V.col(f(0));
            auto b = V.col(f(1));
            auto c = V.col(f(2));

            Eigen::Vector3f ba = b-a;
            Eigen::Vector3f ca = c-a;
            Eigen::Vector3f n = ba.cross(ca);
            //float area = n.norm();
            for(int j = 0; j < 3; ++j) {
                N.col(f(j)) += n;
                //areas(f(j)) += area;
            }
        }
        //N.array().rowwise() /= areas.transpose().array();
        N.colwise().normalize();

        return N;


    }

    void MeshRenderer::setMesh(const MatrixXgf& V, const MatrixXui& F, bool normalize) {
        auto N = computeNormals(V,F);
        setMesh(V,F,N,normalize);
    }
    void MeshRenderer::setMesh(const MatrixXgf& V, const MatrixXui& F, const MatrixXgf& N, bool normalize) {
        setVertices(V,normalize);
        setFaces(F);
        setMeanEdgeLength(V,F,normalize);
        setNormals(N);
    }
    void MeshRenderer::setMeanEdgeLength(const MatrixXgf& V, const MatrixXui& F, bool normalize) {
        float r = 1;

        auto compute_mean_edge_length = [&](const MatrixXgf& V) -> float {
            float ret = 0;
            for(int i = 0; i < F.cols(); ++i) {
                for(int j = 0; j < 3; ++j) {
                    int a = F(j,i);
                    int b = (F(j,i)+1)%3;
                    ret += (V.col(a) - V.col(b)).norm();
                }
            }

            ret /= F.size() * r;

            return ret;
        };
        float mean_edge_length = 0;//technically we want the mean dual edge length
        mean_edge_length = compute_mean_edge_length(V);
        {auto p = baryedge_program()->useRAII();
            baryedge_program()->getUniform("mean_edge_length").set(mean_edge_length);
        }
    }
    void MeshRenderer::setVField(const MatrixXgf& V) {
        if(!buffers()->vectors) {
            buffers()->vectors = std::make_unique<BO>();
        }
        buffers()->vectors->bind();
        buffers()->vectors->setData(V.data(),sizeof(float) * V.size());
        auto vaoraii = vao().enableRAII();


        buffers()->vectors->bind();
        vector_field_program()->getAttrib("vVec").setPointer(m_dim, GL_FLOAT, GL_FALSE, sizeof(float) * m_dim, (void*) 0);
    }
    void MeshRenderer::setVertices(const MatrixXgf& V, bool normalize) {




        buffers()->vertices = std::make_unique<VBO>(GL_POINTS);
        buffers()->vertices->bind();
        if(normalize) {
            update_vertex_scale(V);
        } else {
            m_vertex_scale = 1;
        }
        auto m = V.rowwise().minCoeff();
        auto M = V.rowwise().maxCoeff();
        auto c = (m + M)/2.0;
        MatrixXgf V2 = (V.colwise() - c) / m_vertex_scale;
        buffers()->vertices->setData(V2.data(),sizeof(float) * V2.size());
        std::list<ShaderProgram*> shaders({flat_program().get(), baryedge_program().get()});
        auto m_vaoraii = vao().enableRAII();
        for(auto&& p: shaders) {

            auto active = p->useRAII();

            buffers()->vertices->bind();
            p->getAttrib("vPos").setPointer(m_dim, GL_FLOAT, GL_FALSE, sizeof(float) * m_dim, (void*) 0);
            /*
               program->getUniform("minPos").set3f(minPos.x(),minPos.y(),minPos.z());
               program->getUniform("range").set3f(range.x(),range.y(),range.z());
               */
        }
    }
    void MeshRenderer::setFaces(const MatrixXui& F) {
        if(!buffers()->faces) {
            buffers()->faces = std::make_unique<IBO>(GL_TRIANGLES);
        }
        buffers()->faces->bind();
        buffers()->faces->setData(F.data(),sizeof(int) * F.size());
        setEdgesFromFaces(F);
    }

    void MeshRenderer::setNormals(const MatrixXgf& N) {
        if(m_dim == 3) {

            if(!buffers()->normals) {
                buffers()->normals = std::make_unique<BO>();
            }
            buffers()->normals->bind();
            buffers()->normals->setData(N.data(),sizeof(float) * N.size());



        auto m_vaoraii = vao().enableRAII();

        if(m_dim == 3) {
            auto p = phong_program()->useRAII();
            buffers()->normals->bind();
            phong_program()->getAttrib("vNormal").setPointer(3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);
        }


        }

    }
    void MeshRenderer::setColor(const MatrixXgf& C) {
        auto m_vaoraii = vao().enableRAII();
        if(!buffers()->colors) {
            buffers()->colors = std::make_unique<BO>();
        }


        auto a = vert_color_program()->useRAII();
        buffers()->colors->bind();
        buffers()->colors->setData(C.data(),sizeof(float) * C.size());
        vert_color_program()->getAttrib("vColor").setPointer(3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);
    }
    void MeshRenderer::setEdges(const MatrixXui& E) {

        if(!buffers()->edges) {
            buffers()->edges = std::make_unique<IBO>(GL_LINES);
        }
        buffers()->edges->bind();
        buffers()->edges->setData(E.data(),sizeof(GLuint) * E.size());
    }
    void MeshRenderer::setEdgesFromFaces(const MatrixXui& F) {
        Eigen::Matrix<GLuint,Eigen::Dynamic,Eigen::Dynamic> E;
        E.resize(2,3*F.cols());

        E.leftCols(F.cols()) = F.topRows(2);
        E.block(0,F.cols(),1,F.cols()) = F.row(0);
        E.block(1,F.cols(),1,F.cols()) = F.row(2);
        E.rightCols(F.cols()) = F.bottomRows(2);
        setEdges(E);

    }

    void MeshRenderer::loadShaders(int dim) {





        auto vertex_shader = shaders::simple_vertex_shader(dim);
        auto flat_fragment_shader = shaders::single_color_fragment_shader();
        auto baryedge_fragment_shader = shaders::barycentric_edge_fragment_shader();
        auto baryedge_geometry_shader = shaders::barycentric_edge_geometry_shader();
        auto phong_fragment_shader= shaders::phong_fragment_shader();
        auto vertex_color_fragment_shader = shaders::attribute_color_fragment_shader();

        flat_program() = std::make_unique<ShaderProgram>(linkShaderProgram(vertex_shader,flat_fragment_shader));
        baryedge_program() = std::make_unique<ShaderProgram>(linkShaderProgram(vertex_shader, baryedge_fragment_shader, baryedge_geometry_shader));
        phong_program() = std::make_unique<ShaderProgram>(linkShaderProgram(vertex_shader, phong_fragment_shader));
        vert_color_program() = std::make_unique<ShaderProgram>(linkShaderProgram(vertex_shader,vertex_color_fragment_shader));
        vector_field_program() = std::make_unique<ShaderProgram>(shaders::vector_shader_program(dim, true));

        shaders_enabled() = true;
    }

    void MeshRenderer::imgui_interface() {
        if(ImGui::TreeNode("Mesh Renderer")) {

            static const char* shading_names[] = {
                "Disabled",
                "Flat",
                "Color",
                "Phong",
            };
            int fs = static_cast<int>(m_face_style);
            ImGui::Combo("Shading Style", &fs, shading_names,IM_ARRAYSIZE(shading_names));
            m_face_style = static_cast<FaceStyle>(fs);

            static const char* edge_types[] = {
                "Disabled",
                "BaryEdge",
                "Mesh"
            };
            int et = static_cast<int>(m_edge_type);
            ImGui::Combo("Edge Type", &et, edge_types,IM_ARRAYSIZE(edge_types));
            m_edge_type = static_cast<EdgeType>(et);

            static const char* vertex_names[] = {
                "Disabled",
                "Flat",
                "Color",
            };
            int vs = static_cast<int>(m_vertex_type);
            ImGui::Combo("Vertex Type", &vs, vertex_names,IM_ARRAYSIZE(vertex_names));
            m_vertex_type = static_cast<VertexType>(vs);

            ImGui::Checkbox("Show vfield: ", &m_show_vector_field);


            if(m_face_style == FaceStyle::Phong && ImGui::TreeNode("Phong Shading Parameters")) {
                ImGui::ColorEdit3("ambient", glm::value_ptr(m_ambientMat));
                ImGui::ColorEdit3("diffuse", glm::value_ptr(m_diffuseMat));
                ImGui::ColorEdit3("specular", glm::value_ptr(m_specularMat));
                ImGui::SliderFloat("specular exp", &m_specularExpMat, 0.0,40,"%.4f");
                ImGui::SliderFloat3("light_pos", glm::value_ptr(m_light_pos),-20,20);
                update_phong_shading();
                ImGui::TreePop();
            }
            if(m_face_style == FaceStyle::Flat || m_vertex_type == VertexType::Flat || m_edge_type != EdgeType::Disabled) {
                if(ImGui::TreeNode("Flat Shading Parameters")) {
                    if(m_vertex_type == VertexType::Flat) {
                        ImGui::ColorEdit3("vertex color", glm::value_ptr(m_vertex_color));
                    }
                    if(m_edge_type != EdgeType::Disabled) {
                        ImGui::ColorEdit3("edge color", glm::value_ptr(m_edge_color));
                    }
                    if(m_face_style ==FaceStyle::Flat) {
                        ImGui::ColorEdit3("face color", glm::value_ptr(m_face_color));
                    }
                    ImGui::TreePop();
                }
            }

            if(m_edge_type == EdgeType::BaryEdge) {
                ImGui::SliderFloat("edge_threshold", &m_edge_threshold, 0.0f, 0.01f,"%.5f");
                update_edge_threshold();
            }
            if(m_show_vector_field) {
                ImGui::SliderFloat("Vector Scaling", &m_vector_scale,1e-3,1e1);
                ImGui::ColorEdit3("vector tip color", glm::value_ptr(m_vector_tip_color));
                ImGui::ColorEdit3("vector base color", glm::value_ptr(m_vector_base_color));
                ImGui::SliderFloat("Vector Color Scaling", &m_vector_color_scale,1e-3,1e1);

            }

            ImGui::TreePop();
        }
    }

    void MeshRenderer::render() const {
        if(m_buffers) {
            auto vao_a = vao().enableRAII();
            render(*m_buffers);
        }
    }
    void MeshRenderer::render(const MeshRenderBuffers& buffs) const {

        if(!buffs.vertices) {
            return;
        }
        if(m_vertex_type != VertexType::Disabled) {
            render_points(buffs,m_vertex_type);
        }
        if(m_edge_type != EdgeType::Disabled) {
            render_edges(buffs, m_edge_type);
        }
        if(m_face_style != FaceStyle::Disabled) {
            render_faces(buffs, m_face_style);
        }
        if(m_show_vector_field) {
            render_vfield(buffs);
        }
    }
        void MeshRenderer::render_vfield(const MeshRenderBuffers& buffs) const {
            auto vao_a = vao().enableRAII();
            if(buffs.vectors) {
                glLineWidth(2);
                auto active = vector_field_program()->useRAII();
                vector_field_program()->getUniform("tip_color").setVector(m_vector_tip_color);
                vector_field_program()->getUniform("base_color").setVector(m_vector_base_color);
                vector_field_program()->getUniform("vector_scale").set(m_vector_scale);
                auto vpos_active = vector_field_program()->getAttrib("vPos").enableRAII();
                auto vvel_active = vector_field_program()->getAttrib("vVec").enableRAII();
                buffs.vertices->drawArrays();

            } else {
                mtao::logging::warn() << "vertex velocities not set, can't render vfield" ;
            }
        }

    void MeshRenderer::render_points(const MeshRenderBuffers& buffs, VertexType style) const {
        if(style == VertexType::Flat) {
            auto active = flat_program()->useRAII();
            flat_program()->getUniform("color").setVector(m_vertex_color);

            auto vpos_active = flat_program()->getAttrib("vPos").enableRAII();
            buffs.vertices->drawArrays();
        } else if(style == VertexType::Color) {
            if(!buffs.colors) {
                mtao::logging::warn() << "vertex colors not set, can't render vertices" ;
                return;
            }
            auto active = vert_color_program()->useRAII();

            auto vcol_active = phong_program()->getAttrib("vColor").enableRAII();

            auto vpos_active = vert_color_program()->getAttrib("vPos").enableRAII();
            buffs.vertices->drawArrays();
        }


    }
    void MeshRenderer::render_edges(const MeshRenderBuffers& buffs, EdgeType style) const {

        if(style == EdgeType::BaryEdge) {
            if(!buffs.edges) {
                mtao::logging::warn() << "Face mesh not set, can't render edges with barycentric";
                return;
            }
            auto active = baryedge_program()->useRAII();

            baryedge_program()->getUniform("color").setVector(m_edge_color);
            auto vpos_active = baryedge_program()->getAttrib("vPos").enableRAII();
            buffs.faces->drawElements();

        } else if(style == EdgeType::Mesh) {
            if(!buffs.edges) {
                mtao::logging::warn() << "Edge mesh not set, can't render edges" ;
                return;
            }
            auto active = flat_program()->useRAII();
            flat_program()->getUniform("color").setVector(m_edge_color);

            auto vpos_active = flat_program()->getAttrib("vPos").enableRAII();
            buffs.edges->drawElements();
        }
    }
    void MeshRenderer::render_faces(const MeshRenderBuffers& buffs, FaceStyle style) const {
        if(!buffs.faces) {
            mtao::logging::warn() << "Face mesh not set, can't render faces" ;
            return;
        }

        if(style == FaceStyle::Phong) {
            auto active = phong_program()->useRAII();
            phong_program()->getUniform("color").setVector(m_face_color);

            auto vpos_active = phong_program()->getAttrib("vPos").enableRAII();

            if(buffs.normals) {
                auto vnor_active = phong_program()->getAttrib("vNormal").enableRAII();

                buffs.faces->drawElements();
            } else {

                buffs.faces->drawElements();
            }


        } else if(style == FaceStyle::Flat){
            auto active = flat_program()->useRAII();
            flat_program()->getUniform("color").setVector(m_face_color);

            auto vpos_active = flat_program()->getAttrib("vPos").enableRAII();
            buffs.faces->drawElements();
        } else if(style == FaceStyle::Color) {
            if(!buffs.colors) {
                mtao::logging::warn() << "vertex colors not set, can't render faces" ;
                return;
            }
            auto active = vert_color_program()->useRAII();

            auto vcol_active = phong_program()->getAttrib("vColor").enableRAII();

            auto vpos_active = vert_color_program()->getAttrib("vPos").enableRAII();
            buffs.faces->drawElements();
        }
    }




    void MeshRenderer::update_edge_threshold() {
        auto active = baryedge_program()->useRAII();
        baryedge_program()->getUniform("thresh").set(m_edge_threshold);
    }
    void MeshRenderer::update_phong_shading() {
        auto a = phong_program()->useRAII();
        phong_program()->getUniform("lightPos").setVector(m_light_pos);

        phong_program()->setUniform("specularExpMaterial",m_specularExpMat);
        phong_program()->getUniform("specularMaterial").setVector(m_specularMat);
        phong_program()->getUniform("diffuseMaterial").setVector(m_diffuseMat);
        phong_program()->getUniform("ambientMaterial").setVector(m_ambientMat);
    }

    std::list<ShaderProgram*> MeshRenderer::mvp_programs() const {
        std::list<ShaderProgram*> ret({flat_program().get(), baryedge_program().get(), phong_program().get(), vert_color_program().get(), vector_field_program().get()});
        ret.splice(ret.end(),Renderer::mvp_programs());
        return ret;
    }
}}}
