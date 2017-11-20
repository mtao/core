#pragma once

#include <glm/glm.hpp> 
#include <vector>
#include "imgui.h"

#define ACCESSOR(T,NAME,MEMBER) \
    T& NAME() { return MEMBER; } \
    const T& NAME() const { return MEMBER; }

namespace mtao { namespace opengl {
class Camera {
    public:

        ACCESSOR(glm::mat4,m,m_model)
        ACCESSOR(glm::mat4,v,m_view)
        ACCESSOR(glm::mat4,p,m_perspective)
        ACCESSOR(float,zNear,m_zRange[0]);
        ACCESSOR(float,zFar,m_zRange[1]);

        glm::mat4 mv() const;
        glm::mat4 mvp() const;

        void set_shape(int w, int h);
        float aspect() const;

        void ortho(float distance=1.0f);

        void perspective(float fovy=45.0);


        glm::vec2 get_zRange(const std::vector<glm::vec3>& ) const;
        void set_zRange(const glm::vec2& range ) ;
        void set_zRange(const std::vector<glm::vec3>& ) ;
        glm::vec2 mouse_pos(const glm::vec2& p) const;
        glm::vec2 mouse_pos(float x, float y) const;
        glm::vec2 mouse_pos(const ImVec2& mp) const;
    private:
        glm::mat4 m_model,m_view,m_perspective;
        glm::ivec2 m_shape;
        glm::vec2 m_zRange = glm::vec2(-1.0f,1.0f);
};
}}
#undef ACCESSOR
