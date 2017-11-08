#ifndef OPENGL_UTIL_H
#define OPENGL_UTIL_H
#include <string>
#include <glad/glad.h>
#include "opengl/bind.h"
#include "logging/logger.hpp"
namespace mtao { namespace opengl {
    bool checkOpenGLErrors( const std::string& str = "", bool only_last = false, logging::Level l = logging::Level::Error);
    bool checkOpenGLError( const std::string& str = "", logging::Level l = logging::Level::Error );

}}

#endif//OPENGL_UTIL_H
