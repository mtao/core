#include "mtao/opengl/camera.hpp"
#include <limits>
#include <glm/gtc/matrix_transform.hpp> 
#include <mtao/logging/logger.hpp>


namespace mtao { namespace opengl {

    glm::mat4 Camera::mv() const {
        return v() * m();
    }
    glm::mat4 Camera::mvp() const {
        return p() * mv();
    }
    void Camera::set_shape(int w, int h) {
        m_shape = glm::ivec2(w,h);
    }
    float Camera::aspect() const {
        return m_shape[0] / float(m_shape[1]);
    }
    void Camera::ortho(float distance) {
        float a = aspect();
        p() = glm::ortho(-a * distance, a * distance, -distance, distance, zNear(), zFar());
    }
    void Camera::perspective(float fovy) {
        float a = aspect();
        p() = glm::perspective(fovy,a,zNear(),zFar());
    }

        glm::vec2 Camera::get_zRange(const std::vector<glm::vec3>& pts) const {
            glm::vec2 range(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
            auto mv = this->mv();
            for(auto&& p: pts) {
                float z = (mv * glm::vec4(p,1.0)).z;

                range[0] = std::min(z,range[0]);
                range[1] = std::max(z,range[1]);
            }
            return range;
        }
        void Camera::set_zRange(const glm::vec2& range ) {
            m_zRange = range;
        }
        void Camera::set_zRange(const std::vector<glm::vec3>& pts) {
            set_zRange(get_zRange(pts));
        }
        glm::vec2 Camera::mouse_pos(const ImVec2& p) const {
            return mouse_pos(
                    2*(p.x/m_shape.x -.5)
                    ,
                    -2*(p.y/m_shape.y-.5)
                    );
        }
        glm::vec2 Camera::mouse_pos(float x, float y) const {
            return mouse_pos(glm::vec2(x,y));
        }
        glm::vec2 Camera::mouse_pos(const glm::vec2& p) const {
            auto x = glm::inverse(this->mvp()) * glm::vec4(p,0,1);
            return glm::vec2(x.x,x.y) / x.w;


            return p;
        }

        glm::vec2 Camera::mouse_pos() const {
            auto&& io = ImGui::GetIO();
            return mouse_pos(io.MousePos);
        }
}}
