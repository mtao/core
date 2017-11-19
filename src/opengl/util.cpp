#include "opengl/util.h"
#include <iostream>
#include <glad/glad.h>
#include "logging/logger.hpp"

namespace mtao { namespace opengl {
void process_error(const std::string& str, GLenum err, logging::Level l) {
    std::string errorStr;

#define GL_TOSTR(X) case X: errorStr = #X; break;

    switch(err) {
        GL_TOSTR(GL_INVALID_ENUM)
        GL_TOSTR(GL_INVALID_VALUE)
        GL_TOSTR(GL_INVALID_OPERATION)
        GL_TOSTR(GL_STACK_OVERFLOW)
        GL_TOSTR(GL_STACK_UNDERFLOW)
        GL_TOSTR(GL_OUT_OF_MEMORY)
        GL_TOSTR(GL_INVALID_FRAMEBUFFER_OPERATION)
        GL_TOSTR(GL_CONTEXT_LOST)
        default:
            errorStr = "Unknown error"; break;
    }



    logging::log(l) << "glError[" << str << "]: " << errorStr;
}
bool checkOpenGLErrors(const  std::string& str, bool only_last, logging::Level l) {
    GLenum glErr;
    
    glErr = glGetError();
    if( glErr == GL_NO_ERROR ) {
        return true;
    } else {
        for(;glErr != GL_NO_ERROR; glErr = glGetError() ) {
            if(!only_last) {
                process_error(str,glErr,l);
            }
        }
        if(only_last) {
            process_error(str,glErr,l);
        }
    }

    return false;
}
bool checkOpenGLError( const std::string& str ) {
    return checkOpenGLErrors(str,true);
}
}}