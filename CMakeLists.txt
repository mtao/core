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
    include/mtao/compat.h
    include/mtao/conversion.h
    include/mtao/static_iteration.h
    include/mtao/types.h
    include/mtao/util.h
    include/mtao/eigen_utils.h
    include/mtao/eigen/interweave.h
    include/mtao/eigen/stack.h
    include/mtao/eigen/axial_subspace.h
    include/mtao/type_utils.h
    )

SET(GRID_HEADERS
    ${COMMON_HEADERS}
    include/mtao/geometry/grid/grid.h
    include/mtao/geometry/grid/grid_storage.h
    include/mtao/geometry/grid/grid_utils.h
    include/mtao/geometry/grid/index_map.h
    include/mtao/geometry/grid/index_map_iterator.h
    )

SET(LEVELSET_HEADERS
    ${GRID_HEADERS}
    include/mtao/geometry/levelset/levelset.h
    include/mtao/geometry/levelset/ops.h
    include/mtao/geometry/levelset/levelset_example.h
    include/mtao/geometry/levelset/primitives.h
    include/mtao/geometry/levelset/transformations.h
    include/mtao/geometry/levelset/transformer.h
    include/mtao/geometry/levelset/discrete.h
    include/mtao/geometry/levelset/csg.h
    include/mtao/geometry/levelset/animation.h
    include/mtao/geometry/levelset/particle_sampler.h
    )


ADD_LIBRARY(mtao_core
    include/mtao/logging/logger.hpp
    include/mtao/logging/timer.hpp
    include/mtao/cmdline_parser.hpp
    include/mtao/hotkey_manager.hpp
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
    src/opengl/Monitor.cpp
    src/opengl/bo.cpp
    src/opengl/shader.cpp
    src/opengl/vao.cpp
    src/opengl/util.cpp
    src/opengl/tex.cpp
    src/opengl/camera.cpp
    include/mtao/opengl/imgui_impl.h
    include/mtao/opengl/Window.h
    include/mtao/opengl/Monitor.h
    include/mtao/opengl/bo.h
    include/mtao/opengl/shader.h
    include/mtao/opengl/vao.h
    include/mtao/opengl/util.h
    include/mtao/opengl/tex.h
    include/mtao/opengl/shaders.h
    include/mtao/opengl/camera.hpp

    include/mtao/opengl/renderers/renderer.h
    include/mtao/opengl/renderers/mesh.h
    src/opengl/renderers/mesh.cpp
    src/opengl/renderers/renderer.cpp
    src/opengl/shaders/phong.cpp
    src/opengl/shaders/baryedge.cpp
    src/opengl/shaders/simple.cpp
    src/opengl/shaders/vectors.cpp
    )
TARGET_LINK_LIBRARIES(mtao_opengl ${OPENGL_LIBRARY} ${GLFW_LIBRARIES} dl mtao_core png)


ADD_DEFINITIONS(-g)

ADD_LIBRARY(mtao_geometry
    src/geometry/mesh/halfedge.cpp
    src/geometry/mesh/halfedge_fv_map.cpp
    src/geometry/mesh/read_obj.cpp
    src/geometry/mesh/read_plc.cpp
    include/mtao/geometry/mesh/halfedge.hpp
    include/mtao/geometry/mesh/halfedge_fv_map.hpp
    include/mtao/geometry/mesh/read_obj.hpp
    include/mtao/geometry/mesh/read_plc.hpp
    )

TARGET_LINK_LIBRARIES(mtao_geometry mtao_core)


ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(examples EXCLUDE_FROM_ALL)


