#include "opengl/renderers/mesh.h"
#include <exception>
#include <stdexcept>
#include <sstream>
#include <list>
#include <glad/glad.h>
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace mtao { namespace opengl { namespace renderers {


    bool MeshRenderer::s_shaders_enabled[2] = {false,false};
    std::unique_ptr<ShaderProgram> MeshRenderer::s_flat_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_phong_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_baryedge_program[2];
    std::unique_ptr<ShaderProgram> MeshRenderer::s_vert_color_program[2];
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

        auto compute_mean_edge_length = [&](const MatrixXgf& V) -> float {
            float ret = 0;
            for(int i = 0; i < F.cols(); ++i) {
                for(int j = 0; j < 3; ++j) {
                    int a = F(j,i);
                    int b = (F(j,i)+1)%3;
                    ret += (V.col(a) - V.col(b)).norm();
                }
            }
            ret /= F.size();

            return ret;
        };

        float mean_edge_length = 0;//technically we want the mean dual edge length

        if(!m_vertex_buffer) {
            m_vertex_buffer = std::make_unique<VBO>(GL_POINTS);
        }
        if(!m_index_buffer) {
            m_index_buffer = std::make_unique<IBO>(GL_TRIANGLES);
        }
        m_vertex_buffer->bind();
        if(normalize) {
            auto minPos = V.rowwise().minCoeff();
            auto maxPos = V.rowwise().maxCoeff();
            auto range = maxPos - minPos;

            float r = range.maxCoeff();;
            for(int i = 0; i < range.cols(); ++i) {
                if(range(i) > 1e-5) {
                    r = std::min(r,range(i));
                }
            }

            MatrixXgf V2 = (V.colwise() - (minPos+maxPos)/2) / r;
            m_vertex_buffer->setData(V2.data(),sizeof(float) * V2.size());
            mean_edge_length = compute_mean_edge_length(V2);
        } else {
            m_vertex_buffer->setData(V.data(),sizeof(float) * V.size());
            mean_edge_length = compute_mean_edge_length(V);
        }
        m_index_buffer->bind();
        m_index_buffer->setData(F.data(),sizeof(int) * F.size());

        if(m_dim == 3) {

            if(!m_normal_buffer) {
                m_normal_buffer = std::make_unique<BO>();
            }
            m_normal_buffer->bind();
            m_normal_buffer->setData(N.data(),sizeof(float) * N.size());
        }



        auto m_vaoraii = vao().enableRAII();

        std::list<ShaderProgram*> shaders({flat_program().get(), baryedge_program().get()});
        for(auto&& p: shaders) {

            auto active = p->useRAII();

            m_vertex_buffer->bind();
            p->getAttrib("vPos").setPointer(m_dim, GL_FLOAT, GL_FALSE, sizeof(float) * m_dim, (void*) 0);
            /*
               program->getUniform("minPos").set3f(minPos.x(),minPos.y(),minPos.z());
               program->getUniform("range").set3f(range.x(),range.y(),range.z());
               */
        }
        if(m_dim == 3) {
            auto p = phong_program()->useRAII();
            m_normal_buffer->bind();
            phong_program()->getAttrib("vNormal").setPointer(3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);
        }


        {auto p = baryedge_program()->useRAII();
            baryedge_program()->getUniform("mean_edge_length").set(mean_edge_length);
        }
        setEdgesFromFaces(F);

    }
        void MeshRenderer::setColor(const MatrixXgf& C) {
            if(!m_color_buffer) {
                m_color_buffer = std::make_unique<BO>();
            }
            m_color_buffer->bind();
            vert_color_program()->getAttrib("vColor").setPointer(3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);
            m_color_buffer->setData(C.data(),sizeof(float) * C.size());
        }
    void MeshRenderer::setEdges(const MatrixXui& E) {

        if(!m_edge_index_buffer) {
            m_edge_index_buffer = std::make_unique<IBO>(GL_LINES);
        }
        m_edge_index_buffer->bind();
        m_edge_index_buffer->setData(E.data(),sizeof(GLuint) * E.size());
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

        std::stringstream vss;

        vss <<  "#version 330\n"
            "uniform mat4 MVP;\n";
        if(dim == 2) {
            vss << "in vec2 vPos;\n";
        } else {
            vss << "in vec3 vPos;\n";
            vss << "in vec3 vNormal;\n";
        }
        vss << "in vec3 vColor;\n";
        vss << "out vec3 fNormal;\n";
        vss << "out vec3 fPos;\n";
        vss << "out vec4 gPos;\n";
        vss << "out vec3 fColor;\n";
        vss <<  "void main()\n"
            "{\n";
        if(dim == 2) {
            vss << "    gPos = MVP * vec4(vPos, 0.0, 1.0);\n";
        } else {
            vss << "    gPos = MVP * vec4(vPos, 1.0);\n";
        }
        vss << "    gl_Position = gPos;\n";
        if(dim == 2) {
        vss << "    fPos = vec3(vPos,0);\n";
        vss << "    fNormal = vec3(0,0,1);\n";
        } else {
        vss << "    fPos = vPos;\n";
        vss << "    fNormal = vNormal;\n";
        vss << "    fColor = vColor;\n";
        }
        vss << "}\n";


        static const char* flat_fragment_shader_text =
            "#version 330\n"
            "uniform vec3 color;\n"
            "out vec4 out_color;\n"
            "void main()\n"
            "{\n"
            "    out_color= vec4(color,1.0);\n"
            "}\n";
        static const char* vertex_color_fragment_shader_text =
            "#version 330\n"
            "in vec3 fColor;\n"
            "out vec4 out_color;\n"
            "void main()\n"
            "{\n"
            "    out_color= vec4(fColor,1.0);\n"
            "}\n";

        static const char* baryedge_geometry_shader_text =
            "#version 330\n"
            "layout(triangles) in;\n"
            "layout(triangle_strip,max_vertices = 3) out;\n"
            "in vec4 gPos[3];\n"
            "out vec3 bary;\n"
            "uniform float mean_edge_length;\n"
            "void main()\n"
            "{\n"
            "   int i;"
            "   vec3 lens;"
            "   lens.x = length(gPos[2]-gPos[1]);\n"
            "   lens.y = length(gPos[0]-gPos[2]);\n"
            "   lens.z = length(gPos[0]-gPos[1]);\n"
            "   float s = (lens.x + lens.y + lens.z)/2;\n"
            "   float area = sqrt(s * (s - lens.x) * (s - lens.y) * (s - lens.z));\n"
            "   lens = (2 * area) / lens;\n"
            "   for(i = 0; i < 3; i++)\n"
            "   {\n"
            "       bary = vec3(0);\n"
            "       bary[i] = lens[i] / mean_edge_length;\n"
            //"       bary[i] = 1.0;\n"
            "       gl_Position = gPos[i];\n"
            "       EmitVertex();\n"
            "   }\n"
            "EndPrimitive();\n"
            "}\n";

        static const char* baryedge_fragment_shader_text =
            "#version 330\n"
            "uniform vec3 color;\n"
            "uniform float thresh;\n"
            "in vec3 bary;\n"
            "out vec4 out_color;\n"
            "void main()\n"
            "{\n"
            "   if(min(bary.x,min(bary.y,bary.z)) < thresh){\n"
            "       out_color= vec4(color,1.0);\n"
            "   } else {\n"
            "       discard;\n"
            "   }\n"
            "}\n";

    static const char* phong_fragment_shader_text =
        "#version 330\n"
        //            "uniform vec3 color;\n"
        "uniform mat4 MV;\n"
        "uniform vec4 specularMaterial;\n"
        "uniform float specularExpMaterial;\n"
        "uniform vec4 diffuseMaterial;\n"
        "uniform vec4 ambientMaterial;\n"
        "uniform vec3 lightPos;\n"
        "in vec3 fNormal;\n"
        "in vec3 fPos;\n"
        "out vec4 out_color;\n"
        "void main()\n"
        "{\n"
        "   vec3 ambient = ambientMaterial.xyz;\n"
        "   vec3 eyeDir = normalize(fPos);\n"
        "   vec4 mvpos = MV * vec4(fPos,1);\n"
        "   mvpos = vec4(fPos,1);\n"
        "   vec3 lightDir = normalize(lightPos - mvpos.xyz/mvpos.w);\n"

        "   float lightAng = max(0.0,dot(lightDir,fNormal));\n"
        "   if(lightAng == 0) {\n"
        "       out_color = vec4(ambient,1);\n"
        "       return;\n"
        "   }\n"

        "   vec3 diffuse = lightAng * diffuseMaterial.xyz;\n"

        "   vec3 reflection = normalize(reflect(lightDir, fNormal));\n"
        "   float spec = max(0.0,dot(eyeDir, reflection));\n"
        "   vec3 specular = pow(spec,specularExpMaterial) * specularMaterial.xyz;\n"
        "   out_color= vec4(ambient + diffuse + specular,1.0);\n"
        //"   out_color= vec4(ambient ,1.0);\n"
        //"    out_color= vec4(normal,1.0);\n"
        "}\n";
        auto vertex_shader = prepareShader(vss.str().c_str(), GL_VERTEX_SHADER);
        auto flat_fragment_shader = prepareShader(flat_fragment_shader_text, GL_FRAGMENT_SHADER);
        auto baryedge_fragment_shader = prepareShader(baryedge_fragment_shader_text, GL_FRAGMENT_SHADER);
        auto baryedge_geometry_shader = prepareShader(baryedge_geometry_shader_text, GL_GEOMETRY_SHADER);
        auto phong_fragment_shader= prepareShader(phong_fragment_shader_text, GL_FRAGMENT_SHADER);
        auto vertex_color_fragment_shader = prepareShader(vertex_color_fragment_shader_text, GL_FRAGMENT_SHADER);

        flat_program() = linkShaderProgram(vertex_shader,flat_fragment_shader);
        baryedge_program() = linkShaderProgram(vertex_shader, baryedge_fragment_shader, baryedge_geometry_shader);
        phong_program() = linkShaderProgram(vertex_shader, phong_fragment_shader);
        vert_color_program() = linkShaderProgram(vertex_shader,vertex_color_fragment_shader);

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


            if(m_face_style == FaceStyle::Phong && ImGui::TreeNode("Phong Shading Parameters")) {
                ImGui::ColorEdit3("ambient", glm::value_ptr(m_ambientMat));
                ImGui::ColorEdit3("diffuse", glm::value_ptr(m_diffuseMat));
                ImGui::ColorEdit3("specular", glm::value_ptr(m_specularMat));
                ImGui::SliderFloat("specular exp", &m_specularExpMat, 0.0,40,"%.4f");
                ImGui::SliderFloat3("light_pos", glm::value_ptr(m_light_pos),-20,20);
                update_phong_shading();
                ImGui::TreePop();
            }
            if(m_face_style == FaceStyle::Flat || m_draw_points || m_edge_type != EdgeType::Disabled) {
                if(ImGui::TreeNode("Flat Shading Parameters")) {
                    if(m_draw_points) {
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

            ImGui::SliderFloat("edge_threshold", &m_edge_threshold, 0.0f, 0.01f,"%.5f");
            update_edge_threshold();

            ImGui::Checkbox("Show Vertices", & m_draw_points);
            ImGui::TreePop();
        }
    }

    void MeshRenderer::render() const {
        auto m_vaoraii = vao().enableRAII();

        if(!m_vertex_buffer) {
            return;
        }
        if(m_draw_points) {
            auto active = flat_program()->useRAII();
            flat_program()->getUniform("color").setVector(m_vertex_color);

            m_vertex_buffer->bind();
            auto vpos_active = flat_program()->getAttrib("vPos").enableRAII();
            m_vertex_buffer->drawArrays();

        }
        if(m_edge_type == EdgeType::BaryEdge && m_index_buffer) {
            auto active = baryedge_program()->useRAII();

            baryedge_program()->getUniform("color").setVector(m_edge_color);
            auto vpos_active = baryedge_program()->getAttrib("vPos").enableRAII();
            m_vertex_buffer->bind();
            m_index_buffer->drawElements();

        } else if(m_edge_type == EdgeType::Mesh  && m_edge_index_buffer) {
            auto active = flat_program()->useRAII();
            flat_program()->getUniform("color").setVector(m_edge_color);

            auto vpos_active = flat_program()->getAttrib("vPos").enableRAII();
            m_vertex_buffer->bind();
            m_edge_index_buffer->drawElements();
        }
        if(m_index_buffer) {
            if(m_face_style == FaceStyle::Phong) {
                auto active = phong_program()->useRAII();
                phong_program()->getUniform("color").setVector(m_face_color);

                m_vertex_buffer->bind();
                auto vpos_active = phong_program()->getAttrib("vPos").enableRAII();

                if(m_normal_buffer) {
                    m_normal_buffer->bind();
                    auto vnor_active = phong_program()->getAttrib("vNormal").enableRAII();

                    m_index_buffer->drawElements();
                } else {

                    m_index_buffer->drawElements();
                }


            } else if(m_face_style == FaceStyle::Flat){
                auto active = flat_program()->useRAII();
                flat_program()->getUniform("color").setVector(m_face_color);

                auto vpos_active = flat_program()->getAttrib("vPos").enableRAII();
                m_vertex_buffer->bind();
                m_index_buffer->drawElements();
            } else if(m_face_style == FaceStyle::Color && m_color_buffer) {
                auto active = vert_color_program()->useRAII();

                auto vcol_active = phong_program()->getAttrib("vColor").enableRAII();
                m_color_buffer->bind();

                auto vpos_active = vert_color_program()->getAttrib("vPos").enableRAII();
                m_vertex_buffer->bind();
                m_index_buffer->drawElements();
            }
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
        std::list<ShaderProgram*> ret({flat_program().get(), baryedge_program().get(), phong_program().get(), vert_color_program().get()});
        ret.splice(ret.end(),Renderer::mvp_programs());
        return ret;
    }
}}}
