#ifndef OPENGL_UTIL_H
#define OPENGL_UTIL_H
#include <string>
#include <glad/glad.h>
#include "opengl/bind.h"
namespace mtao { namespace opengl {
    bool checkOpenGLErrors( const std::string& str = "", bool only_last = false);
    bool checkOpenGLError( const std::string& str = "" );

}}

#endif//OPENGL_UTIL_H
