#ifndef OPENGL_UTIL_H
#define OPENGL_UTIL_H
#include <string>
#include "opengl_loader.hpp"
#include "mtao/opengl/bind.h"
#include "mtao/logging/logger.hpp"
namespace mtao { namespace opengl {
    bool checkOpenGLErrors( const std::string& str = "", bool only_last = false, logging::Level l = logging::Level::Error);
    bool checkOpenGLError( const std::string& str = "", logging::Level l = logging::Level::Error );


    struct glEnable_scoped {
        public:
            glEnable_scoped(GLenum e);
            ~glEnable_scoped();
        private:
            GLenum m_enum;
            GLboolean m_initial_enabled;
    };
    struct glDisable_scoped {
        public:
            glDisable_scoped(GLenum e);
            ~glDisable_scoped();
        private:
            GLenum m_enum;
            GLboolean m_initial_enabled;
    };
}}

#endif//OPENGL_UTIL_H
