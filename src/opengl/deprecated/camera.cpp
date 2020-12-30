#include "mtao/opengl/camera.hpp"
#include <limits>
#include <glm/gtc/matrix_transform.hpp>
#include <mtao/logging/logger.hpp>
using namespace mtao::logging;


namespace mtao {
namespace opengl {

    glm::mat4 Camera::mv() const {
        return v() * m();
    }
    glm::mat4 Camera::mvp() const {
        return p() * mv();
    }
    glm::mat4 Camera::mvp_inv_trans() const {
        return glm::transpose(glm::inverse(mvp()));
    }
    void Camera::set_shape(int w, int h) {
        m_shape = glm::ivec2(w, h);
        update();
    }
    float Camera::aspect() const {
        if (m_shape[1] != 0) {
            return m_shape[0] / float(m_shape[1]);
        } else {
            return 1;
        }
    }
    void Camera::ortho(float scale) {
        float a = aspect();
        p() = glm::ortho(-a * scale, a * scale, -scale, scale, zNear(), zFar());
    }
    void Camera::perspective(float fovy) {
        float a = aspect();
        p() = glm::perspective(fovy, a, zNear(), zFar());
    }

    glm::vec2 Camera::get_zRange(const std::vector<glm::vec3> &pts) const {
        glm::vec2 range(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
        auto mv = this->mv();
        for (auto &&p : pts) {
            float z = (mv * glm::vec4(p, 1.0)).z;

            range[0] = std::min(z, range[0]);
            range[1] = std::max(z, range[1]);
        }
        return range;
    }
    void Camera::set_zRange(const glm::vec2 &range) {
        m_zRange = range;
    }
    void Camera::set_zRange(const std::vector<glm::vec3> &pts) {
        set_zRange(get_zRange(pts));
    }
    glm::vec2 Camera::mouse_pos(const ImVec2 &p) const {
        return mouse_pos(
          2 * (p.x / m_shape.x - .5),
          -2 * (p.y / m_shape.y - .5));
    }
    glm::vec2 Camera::mouse_pos(float x, float y) const {
        return mouse_pos(glm::vec2(x, y));
    }
    glm::vec2 Camera::mouse_pos(const glm::vec2 &p) const {
        auto x = glm::inverse(this->mvp()) * glm::vec4(p, 0, 1);
        return glm::vec2(x.x, x.y) / x.w;


        return p;
    }
    Camera2D::Camera2D() {
        zNear() = -1;
        zFar() = 1;
    }

    glm::vec2 Camera::mouse_pos() const {
        auto &&io = ImGui::GetIO();
        return mouse_pos(io.MousePos);
    }

    void Camera2D::set_scale(float scale) {
        m_scale = scale;
        update();
    }
    void Camera2D::set_translation(const glm::vec2 &t) {
        m_translation = t;
        update();
    }
    void Camera2D::update() {
        ortho(m_scale);
    }

    void Camera2D::pan() {
        auto &&io = ImGui::GetIO();
        if (io.KeyShift) {
            m_scale += .1 * io.MouseWheel;
            m_scale = std::max<float>(1e-5, m_scale);
            set_scale(m_scale);
            if (ImGui::IsMouseClicked(0)) {
                enableDrag();
            }
        }
        if (ImGui::IsMouseReleased(0)) {
            disableDrag();
        }
        if (!m_dragMode) {
            return;
        }
        ImVec2 id = io.MouseDelta;
        glm::vec2 dx(
          aspect() * 2 * (id.x / float(shape().x)) * scale(),
          -2 * (id.y / float(shape().y)) * scale());
        if (m_dragMode) {
            m_translation += dx;
        }
    }
    void Camera2D::reset() {
        m_translation = glm::vec2();
    }
    glm::mat4 Camera2D::m() const {
        return glm::translate(Camera::m(), glm::vec3(m_translation, 0));
    }


    void Camera3D::reset() {
        m_translation = glm::vec3();
        m_rotation = glm::vec3();
        m_camera_pos = glm::vec3(0, 0, 5);
        m_target_pos = glm::vec3(0, 0, 0);
        m_camera_up = glm::vec3(0, 1, 0);
        m_distance = glm::length(m_camera_pos - m_target_pos);
    }
    glm::mat4 Camera3D::m() const {
        //auto r = glm::rotate(Camera::m(), m_rotation);
        auto r = glm::rotate(Camera::m(), m_rotation.x, glm::vec3(1.f, 0.f, 0.f));
        r = glm::rotate(r, m_rotation.y, glm::vec3(0.f, 1.f, 0.f));
        r = glm::rotate(r, m_rotation.z, glm::vec3(0.f, 0.f, 1.f));
        return glm::translate(r, m_translation);
    }

    void Camera3D::set_distance(float distance) {
        glm::vec3 d = glm::normalize(camera_pos() - target_pos());
        camera_pos() = target_pos() + distance * d;
        update();
    }
    void Camera3D::update() {
        v() = glm::lookAt(m_camera_pos, m_target_pos, m_camera_up);
        if (m_ortho) {
            ortho();
        } else {
            perspective(m_fov_y);
        }
    }

    void Camera3D::pan() {
        auto &&io = ImGui::GetIO();


        if (io.KeyShift) {
            m_distance += .1 * io.MouseWheel;
            m_distance = std::max<float>(1e-5, m_distance);
            set_distance(m_distance);
            if (!io.WantCaptureMouse) {
                if (ImGui::IsMouseClicked(0)) {
                    enableDrag();
                }
                if (ImGui::IsMouseClicked(1)) {
                    enableAngularDrag();
                }
            }
        }
        if (ImGui::IsMouseReleased(0)) {
            disableDrag();
        }
        if (ImGui::IsMouseReleased(1)) {
            disableAngularDrag();
        }
        if (!(m_dragMode || m_angularDragMode)) {
            return;
        }
        ImVec2 id = io.MouseDelta;
        glm::vec2 dx(
          aspect() * 2 * (id.x / float(shape().x)),
          -2 * (id.y / float(shape().y)));
        if (m_dragMode) {
            m_translation.x += dx.x;
            m_translation.y += dx.y;
        }
        if (m_angularDragMode) {

            m_rotation.y -= dx.x;
            m_rotation.x -= dx.y;
        }
    }


}// namespace opengl
}// namespace mtao
