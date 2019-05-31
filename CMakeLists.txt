CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(MtaoCore)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


SET(CMAKE_RUNTIME_OUTPUT_DIR ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIR ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIR ${PROJECT_BINARY_DIR}/lib)

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
IF(CMAKE_BUILD_TYPE MATCHES Debug)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
ADD_DEFINITIONS(-Wall)
#ADD_DEFINITIONS(-Wno-deprecated-declarations)
#ADD_DEFINITIONS(-Wno-unused-variable)


OPTION(USE_ELTOPO "Should we build the el topo submodule" ON)

OPTION(USE_IMGUI_IMPL "Should the OpenGL Window use the imgui example impl or not" ON)
set(OPENGL_LOADER "GLEW" CACHE STRING "Let us select which OpenGL loader to use")


ADD_DEFINITIONS(-DIMGUI_IMPL_OPENGL_LOADER_${OPENGL_LOADER})
IF(${OPENGL_LOADER} STREQUAL "GLAD") 
    SET(OPENGL_LOADER_SOURCES src/opengl/glad.c)
ENDIF(${OPENGL_LOADER} STREQUAL "GLAD") 

IF(${OPENGL_LOADER} STREQUAL "GLEW") 
    SET(OPENGL_LOADER_LIBRARIES GLEW)
ENDIF(${OPENGL_LOADER} STREQUAL "GLEW") 



# Include dirs
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/extern/triangle)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/extern/triangle)

SET(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)
INCLUDE_DIRECTORIES(${IMGUI_PATH})

SET(IMGUI_IMPL_SOURCES
    ${IMGUI_PATH}/examples/imgui_impl_glfw.cpp
    ${IMGUI_PATH}/examples/imgui_impl_opengl3.cpp
    )

SET(IMGUI_SOURCES 
    ${IMGUI_PATH}/imgui.cpp 
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp
    )

IF(${USE_IMGUI_IMPL})
    SET(IMGUI_SOURCES ${IMGUI_SOURCES} ${IMGUI_IMPL_SOURCES})
    ADD_DEFINITIONS(-DUSE_IMGUI_IMPL)
    ADD_DEFINITIONS(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)
ENDIF(${USE_IMGUI_IMPL})

IF(${USE_ELTOPO})
    ADD_SUBDIRECTORY(extern/eltopo)
    SET(ELTOPO_SOURCES
        src/geometry/mesh/eltopo.cpp
        include/mtao/geometry/mesh/eltopo.h
        )
ENDIF(${USE_ELTOPO})


FIND_PACKAGE(Protobuf)
IF ( PROTOBUF_FOUND )
    INCLUDE_DIRECTORIES( ${PROTOBUF_INCLUDE_DIR} )
    SET(PROTOBUF_IMPORT_DIRS proto)
    PROTOBUF_GENERATE_CPP(GRID_PROTO_SRCS GRID_PROTO_HDRS proto/grid.proto)
    SET(PROTOBUF_LIBS ${PROTOBUF_LIBRARY})
ENDIF(PROTOBUF_FOUND)

FIND_PACKAGE(OpenMP)



SET(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)


FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)


FIND_PACKAGE(PkgConfig REQUIRED)
PKG_SEARCH_MODULE(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})


find_package(Corrade REQUIRED)
find_package(Magnum REQUIRED GL Shaders GlxContext GlfwApplication SceneGraph MeshTools)
find_package(MagnumIntegration REQUIRED ImGui)

FIND_PACKAGE(png++)
IF(png++_FOUND)
    ADD_DEFINITIONS(-DMTAO_HAS_LIBPNGPP)
    include_directories(${png++_INCLUDE_DIRS})
ENDIF(png++_FOUND)
if(OPENGL_FOUND)
    LINK_DIRECTORIES(${OPENGL_LIBRARY_DIRS})
    INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
endif(OPENGL_FOUND)


PKG_SEARCH_MODULE(GLM REQUIRED glm)
include_directories(${GLM_INCLUDE_DIRS})

FIND_PACKAGE(Eigen3 3.3.3 REQUIRED)
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})


ADD_EXECUTABLE(imgui_demo
    ${IMGUI_SOURCES}
    ${IMGUI_IMPL_SOURCES}
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/examples/example_glfw_opengl3/main.cpp
    )
TARGET_LINK_LIBRARIES(imgui_demo ${OPENGL_LIBRARY} ${GLFW_LIBRARIES} ${OPENGL_LOADER_LIBRARIES})
TARGET_INCLUDE_DIRECTORIES(imgui_demo PRIVATE ${IMGUI_PATH}/examples)

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
    include/mtao/logging/profiler.hpp
    include/mtao/cmdline_parser.hpp
    include/mtao/hotkey_manager.hpp
    src/logging/logger.cpp
    src/logging/timer.cpp
    src/logging/profiler.cpp
    src/cmdline_parser.cpp
    src/hotkey_manager.cpp
    )


ADD_LIBRARY(mtao_opengl 
    ${COMMON_HEADERS}
    ${IMGUI_SOURCES}
    ${OPENGL_LOADER_SOURCES}
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

    #Renderers
    src/opengl/renderers/renderer.cpp
    include/mtao/opengl/renderers/renderer.h
    src/opengl/renderers/mesh.cpp
    include/mtao/opengl/renderers/mesh.h
    src/opengl/renderers/bbox.cpp
    include/mtao/opengl/renderers/bbox.h
    src/opengl/renderers/axis.cpp
    include/mtao/opengl/renderers/axis.h
    src/opengl/renderers/tube.cpp
    include/mtao/opengl/renderers/tube.h

    #Shaders
    src/opengl/shaders/phong.cpp
    src/opengl/shaders/baryedge.cpp
    src/opengl/shaders/simple.cpp
    src/opengl/shaders/vectors.cpp
    src/opengl/shaders/tube.cpp
    )


TARGET_LINK_LIBRARIES(mtao_opengl ${OPENGL_LIBRARY} ${GLFW_LIBRARIES} dl mtao_core ${png++_LIBRARIES} ${OPENGL_LOADER_LIBRARIES}
    Magnum::GL
    Magnum::GLContext
    Magnum::Magnum
    Magnum::Shaders
    Magnum::SceneGraph
    Magnum::GlfwApplication
    MagnumIntegration::ImGui
    )


ADD_DEFINITIONS(-O2)

ADD_LIBRARY(mtao_geometry
    src/geometry/mesh/halfedge.cpp
    src/geometry/mesh/halfedge_fv_map.cpp
    src/geometry/mesh/read_obj.cpp
    src/geometry/mesh/triangle/triangle_wrapper.cpp
    src/geometry/mesh/triangle/triangle_wrapper_imgui.cpp
    src/geometry/mesh/triangle/mesh.cpp
    #src/geometry/mesh/read_plc.cpp
    include/mtao/geometry/mesh/halfedge.hpp
    include/mtao/geometry/mesh/halfedge_fv_map.hpp
    include/mtao/geometry/mesh/read_obj.hpp
    #include/mtao/geometry/mesh/read_plc.hpp
    include/mtao/geometry/mesh/boundary_matrix.h
    include/mtao/geometry/mesh/boundary_facets.h
    include/mtao/geometry/strands.h
    src/geometry/strands.cpp
    ${ELTOPO_SOURCES}
    )

TARGET_INCLUDE_DIRECTORIES(mtao_geometry PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern/eltopo)
TARGET_LINK_LIBRARIES(mtao_geometry mtao_core triangle)
IF(USE_ELTOPO)
TARGET_LINK_LIBRARIES(mtao_geometry eltopo)
ENDIF(USE_ELTOPO)
IF(OpenMP_CXX_FOUND)
    TARGET_LINK_LIBRARIES(mtao_geometry OpenMP::OpenMP_CXX)
    TARGET_LINK_LIBRARIES(mtao_opengl OpenMP::OpenMP_CXX)
ENDIF(OpenMP_CXX_FOUND)


ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(examples EXCLUDE_FROM_ALL)


