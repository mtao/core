#include "mtao/opengl/opengl_loader.hpp"
#include "mtao/opengl/renderers/mesh.h"
#include <exception>
#include <stdexcept>
#include <sstream>
#include <array>
#include <list>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include "mtao/opengl/shaders.h"
#include "mtao/logging/logger.hpp"
#include "mtao/geometry/bounding_box.hpp"
#include "mtao/geometry/mesh/vertex_normals.hpp"
using namespace mtao::logging;

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace mtao { namespace opengl { namespace renderers {


    bool MeshRenderer::s_shaders_enabled[2] = {false,false};
    std::unique_ptr<ShaderProgram> MeshRenderer::s_flat_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_phong_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_baryedge_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_vert_color_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_vector_field_program[2];

    MeshRenderer::~MeshRenderer() {}
    MeshRenderer::MeshRenderer(int dim): m_dim(dim) {
        if(dim == 2 || dim == 3) {
            if(!shaders_enabled()) {
                loadShaders(m_dim);
            }
            update_edge_threshold();
            update_phong_shading();
        } else {
            throw std::invalid_argument("Meshes have to be dim 2 or 3");
        }
        m_buffers = std::make_shared<MeshRenderBuffers>();
    }

    void MeshRenderer::update_vertex_scale(const MatrixXgfCRef& V) {
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

    auto MeshRenderer::computeNormals(const MatrixXgfCRef& V, const MatrixXuiCRef& F) -> MatrixXgf {
        return mtao::geometry::mesh::vertex_normals(V,F);


    }

    void MeshRenderer::setMesh(const MatrixXgfCRef& V, const MatrixXuiCRef& F, bool normalize) {
        if(V.size() == 0) {
            warn() << "No vertices sent to setMesh, igoring";
            return;
        }
        if(F.size() == 0) {
            warn() << "No faces sent to setMesh, igoring";
            return;
        }
        assert(F.minCoeff() >= 0);
        assert(F.maxCoeff() < V.cols());
        auto N = computeNormals(V,F);
        setMesh(V,F,N,normalize);
    }
    void MeshRenderer::setMesh(const MatrixXgfCRef& V, const MatrixXuiCRef& F, const MatrixXgfCRef& N, bool normalize) {
        assert(F.rows() > 0);
        assert(F.cols() > 0);
        assert(F.minCoeff() >= 0);
        assert(F.maxCoeff() < V.cols());
        setVertices(V,normalize);
        setFaces(F);
        setMeanEdgeLength(V,F,normalize);
        setNormals(N);
    }
    void MeshRenderer::setMeanEdgeLength(const MatrixXgfCRef& V, const MatrixXuiCRef& F, bool normalize) {
        float r = 1;

        auto compute_mean_edge_length = [&](const MatrixXgfCRef& V) -> float {
            float ret = 0;
            for(int i = 0; i < F.cols(); ++i) {
                for(int j = 0; j < F.rows(); ++j) {
                    int a = F(j,i);
                    int b = (F(j,i)+1)%F.rows();
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
    void MeshRenderer::setVField(const MatrixXgfCRef& V) {
        if(!m_buffers) {
            m_buffers = std::make_shared<MeshRenderBuffers>();
        }
        if(!buffers()->vectors) {
            buffers()->vectors = std::make_unique<BO>();
        }
        buffers()->vectors->bind();
        buffers()->vectors->setData(V.data(),sizeof(float) * V.size());
        auto vaoraii = vao().enableRAII();


        buffers()->vectors->bind();
        vector_field_program()->getAttrib("vVec").setPointer(m_dim, GL_FLOAT, GL_FALSE, sizeof(float) * m_dim, (void*) 0);
    }
    void MeshRenderer::setVertices(const MatrixXgfCRef& V, bool normalize) {
        if(!m_buffers) {
            m_buffers = std::make_shared<MeshRenderBuffers>();
        }

        if(V.size() == 0) {
            buffers()->vertices = nullptr;
        }



        if(!buffers()->vertices) {
            buffers()->vertices = std::make_unique<VBO>(GL_POINTS);
            buffers()->vertices->bind();
        }
        if(normalize) {
            update_vertex_scale(V);
            auto m = V.rowwise().minCoeff();
            auto M = V.rowwise().maxCoeff();
            auto c = (m + M)/2.0;
            MatrixXgf V2 = (V.colwise() - c) / m_vertex_scale;
            buffers()->vertices->setData(V2.data(),sizeof(float) * V2.size());
        } else {
            m_vertex_scale = 1;
            buffers()->vertices->setData(V.data(),sizeof(float) * V.size());
        }
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

    void MeshRenderer::setNormals(const MatrixXgfCRef& N) {
        if(!m_buffers) {
            m_buffers = std::make_shared<MeshRenderBuffers>();
        }
        if(N.size() == 0) {
            buffers()->normals= nullptr;
            return;
        }
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
    void MeshRenderer::setColor(const MatrixXgfCRef& C) {
        if(!m_buffers) {
            m_buffers = std::make_shared<MeshRenderBuffers>();
        }
        if(C.size() == 0) {
            buffers()->colors= nullptr;
            return;
        }
        auto m_vaoraii = vao().enableRAII();
        if(!buffers()->colors) {
            buffers()->colors = std::make_unique<BO>();
        }


        auto a = vert_color_program()->useRAII();
        buffers()->colors->bind();
        buffers()->colors->setData(C.data(),sizeof(float) * C.size());
        vert_color_program()->getAttrib("vColor").setPointer(3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);
    }
    void MeshRenderer::setFaces(const MatrixXgfCRef& V, bool normalize) {
        setVertices(V,normalize);
        m_face_draw_elements = false;
    }
    void MeshRenderer::setEdges(const MatrixXgfCRef& V, bool normalize) {

        setVertices(V,normalize);
        m_edge_draw_elements = false;

    }
    void MeshRenderer::setFaces(const MatrixXuiCRef& F) {
        if(!m_buffers) {
            m_buffers = std::make_shared<MeshRenderBuffers>();
        }
        if(!buffers()->faces) {
            buffers()->faces = std::make_unique<IBO>(GL_TRIANGLES);
        }
        buffers()->faces->bind();
        buffers()->faces->setData(F.data(),sizeof(int) * F.size());
        setEdgesFromFaces(F);
        m_face_draw_elements = true;
    }
    void MeshRenderer::setEdges(const MatrixXuiCRef& E) {
        if(!m_buffers) {
            m_buffers = std::make_shared<MeshRenderBuffers>();
        }
        if(E.size() == 0) {
            buffers()->edges= nullptr;
        }

        if(!buffers()->edges) {
            buffers()->edges = std::make_unique<IBO>(GL_LINES);
        }
        buffers()->edges->bind();
        buffers()->edges->setData(E.data(),sizeof(GLuint) * E.size());
        m_edge_draw_elements = true;
    }
    void MeshRenderer::setEdgesFromFaces(const MatrixXuiCRef& F) {
        MatrixXui  E(2,3*F.cols());

        E.leftCols(F.cols()) = F.topRows(2);
        E.block(0,F.cols(),1,F.cols()) = F.row(0);
        E.block(1,F.cols(),1,F.cols()) = F.row(2);
        E.rightCols(F.cols()) = F.bottomRows(2);
        setEdges(E);

    }

    void MeshRenderer::loadShaders(int dim) {





        auto vertex_shader = shaders::simple_vertex_shader(m_dim);
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

    void MeshRenderer::imgui_interface(const std::string& name) {
        if(ImGui::TreeNode(name.c_str())) {

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
                "Mesh",
                "Color"
            };
            int et = static_cast<int>(m_edge_type);
            ImGui::Combo("Edge Type", &et, edge_types,IM_ARRAYSIZE(edge_types));
            m_edge_type = static_cast<EdgeStyle>(et);

            static const char* vertex_names[] = {
                "Disabled",
                "Flat",
                "Color",
            };
            int vs = static_cast<int>(m_vertex_type);
            ImGui::Combo("Vertex Type", &vs, vertex_names,IM_ARRAYSIZE(vertex_names));
            m_vertex_type = static_cast<VertexStyle>(vs);

            ImGui::Checkbox("Show vfield: ", &m_show_vector_field);

            if(m_edge_type != EdgeStyle::Disabled && m_edge_type != EdgeStyle::BaryEdge) {
                static std::array<float,2> range = get_line_width_range();
                {
                    bool tmp = m_use_line_smooth;
                    ImGui::Checkbox("Use Antialiasing: ", &tmp);
                    if(tmp != m_use_line_smooth) {
                        m_use_line_smooth = tmp;
                        range = get_line_width_range();
                    }
                }
                ImGui::SliderFloat("Line Width",&m_line_width, range[0],range[1]);
            }
            if(m_vertex_type != VertexStyle::Disabled) {
                static std::array<float,2> range = get_point_size_range();
                ImGui::SliderFloat("Point Size",&m_point_size,range[0],range[1]);
            }

            if(m_face_style == FaceStyle::Phong && ImGui::TreeNode("Phong Shading Parameters")) {
                ImGui::ColorEdit3("ambient", glm::value_ptr(m_ambientMat));
                ImGui::ColorEdit3("diffuse", glm::value_ptr(m_diffuseMat));
                ImGui::ColorEdit3("specular", glm::value_ptr(m_specularMat));
                ImGui::SliderFloat("specular exp", &m_specularExpMat, 0.0,40,"%.4f");
                ImGui::SliderFloat3("light_pos", glm::value_ptr(m_light_pos),-20,20);
                update_phong_shading();
                ImGui::TreePop();
            }
            if(m_face_style == FaceStyle::Flat || m_vertex_type == VertexStyle::Flat || m_edge_type != EdgeStyle::Disabled) {
                if(ImGui::TreeNode("Flat Shading Parameters")) {
                    if(m_vertex_type == VertexStyle::Flat) {
                        ImGui::ColorEdit4("vertex color", glm::value_ptr(m_vertex_color));
                    }
                    if(m_edge_type != EdgeStyle::Disabled && m_edge_type != EdgeStyle::Color) {
                        ImGui::ColorEdit4("edge color", glm::value_ptr(m_edge_color));
                    }
                    if(m_face_style ==FaceStyle::Flat) {
                        ImGui::ColorEdit4("face color", glm::value_ptr(m_face_color));
                    }
                    ImGui::TreePop();
                }
            }

            if(m_edge_type == EdgeStyle::BaryEdge) {
                ImGui::SliderFloat("edge_threshold", &m_edge_threshold, 0.0f, 0.01f,"%.5f");
                update_edge_threshold();
            }
            if(m_show_vector_field) {
                ImGui::SliderFloat("Vector Scaling", &m_vector_scale,1e-5,1e0,"%.5f");
                ImGui::ColorEdit4("vector tip color", glm::value_ptr(m_vector_tip_color));
                ImGui::ColorEdit4("vector base color", glm::value_ptr(m_vector_base_color));
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
    void MeshRenderer::render_points() const {
        glEnable(GL_POLYGON_OFFSET_POINT);
        glPolygonOffset(3,1);
        if(m_buffers) {
            auto vao_a = vao().enableRAII();
            if(m_vertex_type != VertexStyle::Disabled) {
                render_points(*m_buffers,m_vertex_type);
            }
        }
    }
    void MeshRenderer::render_edges() const {
        glEnable(GL_POLYGON_OFFSET_LINE);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1,1);
        if(m_buffers) {
            auto vao_a = vao().enableRAII();
            if(m_edge_type != EdgeStyle::Disabled) {
                render_edges(*m_buffers, m_edge_type);
            }
        }
    }
    void MeshRenderer::render_faces() const {
        if(m_buffers) {
            glPolygonOffset(0,0);
            auto vao_a = vao().enableRAII();
            if(m_face_style != FaceStyle::Disabled) {
                render_faces(*m_buffers, m_face_style);
            }
        }
    }
    void MeshRenderer::render_vfield() const {
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(2,1);
        if(m_buffers) {
            auto vao_a = vao().enableRAII();
            if(m_show_vector_field) {
                render_vfield(*m_buffers);
            }
        }
    }
    void MeshRenderer::render(const MeshRenderBuffers& buffs) const {

        if(!buffs.vertices) {
            return;
        }
        if(m_face_style != FaceStyle::Disabled) {
            render_faces(buffs, m_face_style);
        }
        if(m_edge_type != EdgeStyle::Disabled) {
            render_edges(buffs, m_edge_type);
        }
        if(m_vertex_type != VertexStyle::Disabled) {
            render_points(buffs,m_vertex_type);
        }
        if(m_show_vector_field) {
            render_vfield(buffs);
        }
    }
        void MeshRenderer::render_vfield(const MeshRenderBuffers& buffs) const {
            auto vao_a = vao().enableRAII();

            if(buffs.vectors) {
                glLineWidth(line_width());
                if(m_use_line_smooth) {
                    glEnable(GL_LINE_SMOOTH);
                } else {
                    glDisable(GL_LINE_SMOOTH);
                }
                auto active = vector_field_program()->useRAII();
                vector_field_program()->getUniform("tip_color").setVector(m_vector_tip_color);
                vector_field_program()->getUniform("base_color").setVector(m_vector_base_color);
                vector_field_program()->getUniform("vector_scale").set(m_vector_scale);
                auto vpos_active = vector_field_program()->getAttrib("vPos").enableRAII();
                auto vvel_active = vector_field_program()->getAttrib("vVec").enableRAII();
                buffs.vertices->drawArraysStride(m_dim);

            } else {
                mtao::logging::warn() << "vertex velocities not set, can't render vfield" ;
            }
        }

    void MeshRenderer::render_points(const MeshRenderBuffers& buffs, VertexStyle style) const {
        glPointSize(point_size());
        if(!buffs.vertices) {
            mtao::logging::warn() << "vertex positions not set, can't render points" ;
            return;
        }
        if(style == VertexStyle::Flat) {
            auto active = flat_program()->useRAII();
            flat_program()->getUniform("color").setVector(m_vertex_color);

            auto vpos_active = flat_program()->getAttrib("vPos").enableRAII();
            buffs.vertices->drawArraysStride(m_dim);
        } else if(style == VertexStyle::Color) {
            if(!buffs.colors) {
                mtao::logging::warn() << "vertex colors not set, can't render vertices" ;
                return;
            }
            auto active = vert_color_program()->useRAII();

            auto vcol_active = phong_program()->getAttrib("vColor").enableRAII();

            auto vpos_active = vert_color_program()->getAttrib("vPos").enableRAII();
            buffs.vertices->drawArraysStride(m_dim);
        }


    }
    void MeshRenderer::drawEdges(const MeshRenderBuffers& buffs) const {
        auto s = glEnable_scoped(GL_POLYGON_OFFSET_LINE);
        if(m_edge_draw_elements) {
            if(!buffs.edges) {
                mtao::logging::warn() << "Face mesh not set, can't render edges with barycentric";
                return;
            }
            buffs.edges->drawElements();
        } else {
            buffs.vertices->drawArraysStride(m_dim, GL_LINES);
        }
    }
    void MeshRenderer::drawFaces(const MeshRenderBuffers& buffs) const {
        if(m_face_draw_elements) {
            if(!buffs.faces) {
                mtao::logging::warn() << "Face mesh not set, can't render faces" ;
                return;
            }
            buffs.faces->drawElements();
        } else {
            buffs.vertices->drawArraysStride(m_dim, GL_TRIANGLES);
        }
    }
    void MeshRenderer::render_edges(const MeshRenderBuffers& buffs, EdgeStyle style) const {
        glLineWidth(line_width());
        if(m_use_line_smooth) {
            glEnable(GL_LINE_SMOOTH);
        } else {
            glDisable(GL_LINE_SMOOTH);
        }
        if(!buffs.vertices) {
            return;
        }

        if(style == EdgeStyle::BaryEdge) {
            auto active = baryedge_program()->useRAII();

            baryedge_program()->getUniform("color").setVector(m_edge_color);
            auto vpos_active = baryedge_program()->getAttrib("vPos").enableRAII();
            drawFaces(buffs);

        } else if(style == EdgeStyle::Mesh) {
            auto active = flat_program()->useRAII();
            flat_program()->getUniform("color").setVector(m_edge_color);

            auto vpos_active = flat_program()->getAttrib("vPos").enableRAII();
            drawEdges(buffs);
        } else if(style == EdgeStyle::Color) {
            if(!buffs.colors) {
                mtao::logging::warn() << "vertex colors not set, can't render edges" ;
                return;
            }
            auto active = vert_color_program()->useRAII();

            auto vcol_active = phong_program()->getAttrib("vColor").enableRAII();

            auto vpos_active = vert_color_program()->getAttrib("vPos").enableRAII();
            drawEdges(buffs);
        }
    }
    void MeshRenderer::render_faces(const MeshRenderBuffers& buffs, FaceStyle style) const {
        if(!buffs.vertices) {
            return;
        }

        if(style == FaceStyle::Phong) {
            auto active = phong_program()->useRAII();
            phong_program()->getUniform("color").setVector(m_face_color);

            auto vpos_active = phong_program()->getAttrib("vPos").enableRAII();

            if(buffs.normals) {
                auto vnor_active = phong_program()->getAttrib("vNormal").enableRAII();

                drawFaces(buffs);
            } else {

                drawFaces(buffs);
            }


        } else if(style == FaceStyle::Flat){
            auto active = flat_program()->useRAII();
            flat_program()->getUniform("color").setVector(m_face_color);

            auto vpos_active = flat_program()->getAttrib("vPos").enableRAII();
            drawFaces(buffs);
        } else if(style == FaceStyle::Color) {
            if(!buffs.colors) {
                mtao::logging::warn() << "vertex colors not set, can't render faces" ;
                return;
            }
            auto active = vert_color_program()->useRAII();

            auto vcol_active = phong_program()->getAttrib("vColor").enableRAII();

            auto vpos_active = vert_color_program()->getAttrib("vPos").enableRAII();
            drawFaces(buffs);
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
    void MeshRenderer::unset_all() {
        set_face_style(FaceStyle::Disabled);
        set_edge_style(EdgeStyle::Disabled);
        set_vertex_style(VertexStyle::Disabled);
        show_vector_field(false);
    }

    std::array<float,2> MeshRenderer::get_line_width_range() const {
        std::array<float,2> ret;
        GLenum pname = m_use_line_smooth?GL_SMOOTH_LINE_WIDTH_RANGE:GL_ALIASED_LINE_WIDTH_RANGE;
        glGetFloatv(pname,&ret[0]);
        return ret;
    }
    std::array<float,2> MeshRenderer::get_point_size_range() const {
        std::array<float,2> ret;
        glGetFloatv(GL_POINT_SIZE_RANGE,&ret[0]);
        return ret;
    }
}}}
