#pragma once

#include "../util.h"
#include <glm/glm.hpp> 
#include <vector>
#include "imgui.h"

namespace mtao { namespace opengl {
class Camera {
    public:

        virtual glm::mat4 m() const {return m_model; }
        glm::mat4& m() { return m_model; }
        void set_m(const glm::mat4& a) { m_model = a; }

        //MTAO_ACCESSORS(glm::mat4,m,m_model)
        MTAO_ACCESSORS(glm::mat4,v,m_view)
        MTAO_ACCESSORS(glm::mat4,p,m_perspective)
        MTAO_ACCESSORS(float,zNear,m_zRange[0]);
        MTAO_ACCESSORS(float,zFar,m_zRange[1]);
        MTAO_ACCESSOR_CONST(glm::ivec2,shape,m_shape);

        glm::mat4 mv() const;
        glm::mat4 mvp() const;
        glm::mat4 mvp_inv_trans() const;

        void set_shape(int w, int h);
        float aspect() const;

        void ortho(float scale=1.0f);

        void perspective(float fovy=45.0);
        virtual void update() {}


        glm::vec2 get_zRange(const std::vector<glm::vec3>& ) const;
        void set_zRange(const glm::vec2& range ) ;
        void set_zRange(const std::vector<glm::vec3>& ) ;
        glm::vec2 mouse_pos(const glm::vec2& p) const;
        glm::vec2 mouse_pos(float x, float y) const;
        glm::vec2 mouse_pos(const ImVec2& mp) const;
        glm::vec2 mouse_pos() const;
    private:
        glm::mat4 m_model,m_view,m_perspective;
        glm::ivec2 m_shape;
        glm::vec2 m_zRange = glm::vec2(0.1,10.0f);
};

class Camera2D: public Camera {
    public:

        Camera2D();
        float& scale() { return m_scale; }
        float scale() const { return m_scale; }
        void set_scale(float scale);
        void set_translation(const glm::vec2& t);
        void update() override;
        void pan();
        void enableDrag() { m_dragMode = true;}
        void disableDrag() { m_dragMode = false; }
        void reset(); 
        glm::mat4 m() const override;

    private:
        float m_scale = 1.0;
        bool m_dragMode = false;
        glm::vec2 m_translation;
};
class Camera3D: public Camera {
    public:

        glm::vec3& camera_pos() { return m_camera_pos; }
        glm::vec3& target_pos() { return m_target_pos; }
        glm::vec3& camera_up() { return m_camera_up; }
        const glm::vec3& camera_pos() const { return m_camera_pos; }
        const glm::vec3& target_pos() const { return m_target_pos; }
        const glm::vec3& camera_up() const { return m_camera_up; }
        void set_distance(float distance);
        
        void set_ortho() { m_ortho = true; }
        void set_perspective() { m_ortho = false; }
        void update() override;
        void pan();
        void enableDrag() { m_dragMode = true;}
        void disableDrag() { m_dragMode = false; }
        void enableAngularDrag() { m_angularDragMode = true;}
        void disableAngularDrag() { m_angularDragMode = false; }
        void reset(); 
        glm::mat4 m() const override;

    private:
        glm::vec3 m_camera_pos = glm::vec3(0,0,5);
        glm::vec3 m_target_pos = glm::vec3(0,0,0);
        glm::vec3 m_camera_up = glm::vec3(0,1,0);
        bool m_ortho = false;
        bool m_dragMode = false;
        bool m_angularDragMode = false;
        float m_fov_y = 45.0;
        float m_distance = 5.0;
        glm::vec3 m_translation;
        glm::vec3 m_rotation;
};
}}
#undef ACCESSOR
