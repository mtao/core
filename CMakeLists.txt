cmake_minimum_required(VERSION 2.8.9)
PROJECT(MtaoCore)

SET(CMAKE_RUNTIME_OUTPUT_DIR ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIR ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIR ${PROJECT_BINARY_DIR}/lib)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++1z)
IF(CMAKE_BUILD_TYPE MATCHES Debug)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
ADD_DEFINITIONS(-Wall)
#ADD_DEFINITIONS(-Wno-deprecated-declarations)
#ADD_DEFINITIONS(-Wno-unused-variable)



# Include dirs
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(/usr/include/eigen3)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)
INCLUDE_DIRECTORIES(${IMGUI_PATH})
SET(IMGUI_SOURCES ${IMGUI_PATH}/imgui.cpp ${IMGUI_PATH}/imgui_draw.cpp)



FIND_PACKAGE(Protobuf)
IF ( PROTOBUF_FOUND )
    INCLUDE_DIRECTORIES( ${PROTOBUF_INCLUDE_DIR} )
    SET(PROTOBUF_IMPORT_DIRS proto)
    PROTOBUF_GENERATE_CPP(GRID_PROTO_SRCS GRID_PROTO_HDRS proto/grid.proto)
    SET(PROTOBUF_LIBS ${PROTOBUF_LIBRARY})
ENDIF(PROTOBUF_FOUND)




FIND_PACKAGE(OpenGL REQUIRED)
if(OPENGL_FOUND)
    LINK_DIRECTORIES(${OPENGL_LIBRARY_DIRS})
    INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
endif(OPENGL_FOUND)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_SEARCH_MODULE(GLFW REQUIRED glfw3)


INCLUDE_DIRECTORIES(${GLFW3_INCLUDE_DIRS})




SET(COMMON_HEADERS
    include/compat.h
    include/conversion.h
    include/static_iteration.h
    include/types.h
    include/util.h
    include/eigen_utils.h
    include/type_utils.h
    )

SET(GRID_HEADERS
    ${COMMON_HEADERS}
    include/geometry/grid/grid.h
    include/geometry/grid/grid_storage.h
    include/geometry/grid/grid_utils.h
    include/geometry/grid/index_map.h
    include/geometry/grid/index_map_iterator.h
    )

SET(LEVELSET_HEADERS
    ${GRID_HEADERS}
    include/geometry/levelset/levelset.h
    include/geometry/levelset/ops.h
    include/geometry/levelset/levelset_example.h
    include/geometry/levelset/primitives.h
    include/geometry/levelset/transformations.h
    include/geometry/levelset/transformer.h
    include/geometry/levelset/discrete.h
    include/geometry/levelset/csg.h
    include/geometry/levelset/animation.h
    include/geometry/levelset/particle_sampler.h
    )


ADD_LIBRARY(mtao_core
    include/logging/logger.hpp
    include/logging/timer.hpp
    include/cmdline_parser.hpp
    include/hotkey_manager.hpp
    src/logging/logger.cpp
    src/logging/timer.cpp
    src/cmdline_parser.cpp
    src/hotkey_manager.cpp
    )


ADD_LIBRARY(mtao_opengl 
    ${COMMON_HEADERS}
    ${IMGUI_SOURCES}
    src/opengl/glad.c
    src/opengl/imgui_impl.cpp
    src/opengl/Window.cpp
    src/opengl/bo.cpp
    src/opengl/shader.cpp
    src/opengl/vao.cpp
    src/opengl/util.cpp
    src/opengl/tex.cpp
    src/opengl/camera.cpp
    include/opengl/imgui_impl.h
    include/opengl/Window.h
    include/opengl/bo.h
    include/opengl/shader.h
    include/opengl/vao.h
    include/opengl/util.h
    include/opengl/tex.h
    include/opengl/shaders.h
    include/opengl/camera.hpp

    include/opengl/renderers/renderer.h
    include/opengl/renderers/mesh.h
    src/opengl/renderers/mesh.cpp
    src/opengl/renderers/renderer.cpp
    src/opengl/shaders/phong.cpp
    src/opengl/shaders/baryedge.cpp
    src/opengl/shaders/simple.cpp
    src/opengl/shaders/vectors.cpp
    )
TARGET_LINK_LIBRARIES(mtao_opengl ${OPENGL_LIBRARY} ${GLFW_LIBRARIES} dl mtao_core)


ADD_DEFINITIONS(-g)

ADD_LIBRARY(mtao_geometry
    src/geometry/mesh/halfedge.cpp
    src/geometry/mesh/halfedge_fv_map.cpp
    src/geometry/mesh/read_obj.cpp
    include/geometry/mesh/halfedge.hpp
    include/geometry/mesh/halfedge_fv_map.hpp
    include/geometry/mesh/read_obj.hpp
    )

TARGET_LINK_LIBRARIES(mtao_geometry mtao_core)


ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(examples EXCLUDE_FROM_ALL)


