#include "opengl/util.h"
#include <iostream>
#include <glad/glad.h>

namespace mtao { namespace opengl {
void process_error(const std::string& str, GLenum err) {
    std::string errorStr;


    switch(err) {
        case GL_INVALID_ENUM:
            errorStr = "Invalid Enum"; break;
        case GL_INVALID_VALUE:
            errorStr = "Invalid value"; break;
        case GL_INVALID_OPERATION:
            errorStr = "Invalid operation"; break;
        case GL_STACK_OVERFLOW:
            errorStr = "Stack overflow"; break;
        case GL_STACK_UNDERFLOW:
            errorStr = "Stack underflow"; break;
        case GL_OUT_OF_MEMORY:
            errorStr = "Out of memory"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
        case GL_CONTEXT_LOST:
            errorStr = "Context lost"; break;
        default:
            errorStr = "Unknown error"; break;
    }



    std::cout << "glError[" << str << "]: " << errorStr << std::endl;
}
bool checkOpenGLErrors(const  std::string& str, bool only_last ) {
    GLenum glErr;
    
    glErr = glGetError();
    if( glErr == GL_NO_ERROR ) {
        return true;
    } else {
        for(;glErr != GL_NO_ERROR; glErr = glGetError() ) {
            if(!only_last) {
                process_error(str,glErr);
            }
        }
        if(only_last) {
            process_error(str,glErr);
        }
    }

    return false;
}
bool checkOpenGLError( const std::string& str ) {
    return checkOpenGLErrors(str,true);
}
}}
