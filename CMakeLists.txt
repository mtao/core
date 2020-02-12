CMAKE_MINIMUM_REQUIRED(VERSION 3.13.4)
PROJECT(Mtao)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()


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


OPTION(MTAO_USE_ELTOPO "Should we build the el topo submodule" ON)
OPTION(MTAO_USE_LOSTOPOS "Should we build the los topos submodule" OFF)
OPTION(MTAO_USE_OPENGL "Build opengl stuff" ON)
OPTION(MTAO_USE_PNGPP "Use PNG++ for screenshots" ON)

include(mtao_fetch_dependencies)

FIND_PACKAGE(Eigen3 3.3.3 REQUIRED)





# Include dirs

SET(CMAKE_INCLUDE_CURRENT_DIR ON)


FIND_PACKAGE(OpenMP REQUIRED)
SET(OpenMP_TARGET OpenMP::OpenMP_CXX)


######Geometry


IF(MTAO_USE_ELTOPO)
    SET(ELTOPO_SOURCES
        src/geometry/mesh/eltopo.cpp
        include/mtao/geometry/mesh/eltopo.h
        )
ENDIF(MTAO_USE_ELTOPO)


IF(MTAO_USE_LOSTOPOS)
    SET(LOSTOPOS_SOURCES
        src/geometry/mesh/lostopos.cpp
        include/mtao/geometry/mesh/lostopos.hpp
        )
ENDIF(MTAO_USE_LOSTOPOS)

#FIND_PACKAGE(Protobuf)
#IF ( PROTOBUF_FOUND )
#    INCLUDE_DIRECTORIES( ${PROTOBUF_INCLUDE_DIR} )
#    SET(PROTOBUF_IMPORT_DIRS proto)
#    PROTOBUF_GENERATE_CPP(GRID_PROTO_SRCS GRID_PROTO_HDRS proto/grid.proto)
#    SET(PROTOBUF_LIBS ${PROTOBUF_LIBRARY})
#ENDIF(PROTOBUF_FOUND)





######Visualziation

IF(MTAO_USE_OPENGL)

    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL REQUIRED)
    #find_package(Corrade REQUIRED Utility)
    #find_package(Magnum COMPONENTS GL Shaders GlxContext GlfwApplication SceneGraph MeshTools)
    #find_package(MagnumIntegration COMPONENTS ImGui)


    #FIND_PACKAGE(PkgConfig REQUIRED)
    #PKG_SEARCH_MODULE(GLFW REQUIRED glfw3)
    #include_directories(${GLFW_INCLUDE_DIRS})

    #PKG_SEARCH_MODULE(GLM REQUIRED glm)
    #include_directories(${GLM_INCLUDE_DIRS})
ENDIF(MTAO_USE_OPENGL)









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

add_library(mtao_headers INTERFACE)
target_include_directories(mtao_headers INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
TARGET_LINK_LIBRARIES(mtao_headers PUBLIC INTERFACE Eigen3::Eigen)

ADD_LIBRARY(mtao_common 
    include/mtao/logging/logger.hpp
    include/mtao/logging/timer.hpp
    include/mtao/logging/profiler.hpp
    include/mtao/cmdline_parser.hpp
    src/logging/logger.cpp
    src/logging/timer.cpp
    src/logging/profiler.cpp
    src/cmdline_parser.cpp
    )


TARGET_LINK_LIBRARIES(mtao_common mtao_headers Eigen3::Eigen spdlog::spdlog)

IF(MTAO_USE_OPENGL)
    CORRADE_ADD_RESOURCE(MtaoShaders ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/shaders/resources.conf)

ADD_LIBRARY(mtao_visualization 
    ${MtaoShaders}
    ${COMMON_HEADERS}
    src/opengl/Window.cpp
    src/opengl/Window2.cpp
    src/opengl/Window3.cpp
    src/opengl/Monitor.cpp
    #src/opengl/bo.cpp
    #src/opengl/shader.cpp
    #src/opengl/vao.cpp
    #src/opengl/util.cpp
    #src/opengl/tex.cpp
    #src/opengl/camera.cpp
    #include/mtao/opengl/Window.h
    #include/mtao/opengl/Monitor.h
    #include/mtao/opengl/bo.h
    #include/mtao/opengl/shader.h
    #include/mtao/opengl/vao.h
    #include/mtao/opengl/util.h
    #include/mtao/opengl/tex.h
    #include/mtao/opengl/shaders.h
    #include/mtao/opengl/camera.hpp
    src/hotkey_manager.cpp
    include/mtao/hotkey_manager.hpp

    src/opengl/objects/mesh.cpp
    src/opengl/shaders.cpp
    src/opengl/drawables.cpp


    #Shaders
    src/opengl/shaders/vector_field.cpp
    )
if(MTAO_USE_PNGPP)
    FIND_PACKAGE(png++ REQUIRED)
    TARGET_COMPILE_DEFINITIONS(mtao_visualization PRIVATE -DMTAO_HAS_LIBPNGPP)
    TARGET_INCLUDE_DIRECTORIES(mtao_visualization PRIVATE ${png++_INCLUDE_DIRS})
endif(MTAO_USE_PNGPP)

TARGET_LINK_LIBRARIES(mtao_visualization PUBLIC mtao_common 
    Magnum::GL
    Magnum::Magnum
    Magnum::Shaders
    Magnum::SceneGraph
    Magnum::MeshTools
    Magnum::GlfwApplication
    MagnumIntegration::ImGui
    Corrade::Utility
    ImGui::ImGui
    PRIVATE OpenGL::GL GLFW::GLFW dl ${png++_LIBRARIES} 
    )
ENDIF(MTAO_USE_OPENGL)



SET(GEOMETRY_SRCS
    src/geometry/mesh/halfedge.cpp
    src/geometry/mesh/halfedge_fv_map.cpp
    src/geometry/mesh/read_obj.cpp
    src/geometry/point_cloud/read_xyz.cpp
    src/geometry/mesh/write_obj.cpp
    #src/geometry/mesh/read_ply.cpp
    src/geometry/mesh/write_ply.cpp
    src/geometry/mesh/triangle/triangle_wrapper.cpp
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
    ${LOSTOPOS_SOURCES}
    )
IF(MTAO_USE_OPENGL)
    SET(GEOMETRY_SRCS ${GEOMETRY_SRCS}
        src/geometry/mesh/triangle/triangle_wrapper_imgui.cpp)
ENDIF(MTAO_USE_OPENGL)

ADD_LIBRARY(mtao_geometry 
    ${GEOMETRY_SRCS}
    )

IF(MTAO_USE_OPENGL)
    SET(GEOMETRY_EXTRAS ${GEOMETRY_EXTRAS} ImGui::ImGui MagnumIntegration::ImGui)
ENDIF(MTAO_USE_OPENGL)
IF(MTAO_USE_ELTOPO)
    SET(GEOMETRY_EXTRAS ${GEOMETRY_EXTRAS} eltopo)
ENDIF(MTAO_USE_ELTOPO)
IF(MTAO_USE_LOSTOPOS)
    SET(GEOMETRY_EXTRAS ${GEOMETRY_EXTRAS} LosTopos)
ENDIF(MTAO_USE_LOSTOPOS)
TARGET_LINK_LIBRARIES(mtao_geometry PUBLIC mtao_common ${OpenMP_TARGET} PRIVATE triangle ${GEOMETRY_EXTRAS})
TARGET_INCLUDE_DIRECTORIES(mtao_geometry PRIVATE ${triangle_INCLUDE_DIR})


#SET(MTAO_EXT_LIBS mtao::common mtao::opengl mtao::geometry)
#SET(MTAO_EXT_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include)

ADD_SUBDIRECTORY(tools EXCLUDE_FROM_ALL)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    ADD_SUBDIRECTORY(tests)
endif()
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
ADD_SUBDIRECTORY(examples EXCLUDE_FROM_ALL)
endif()

add_library(mtao::headers ALIAS mtao_headers)
add_library(mtao::common ALIAS mtao_common)
add_library(mtao::geometry ALIAS mtao_geometry)
IF(MTAO_USE_OPENGL)
  add_library(mtao::visualization ALIAS mtao_visualization)
ENDIF(MTAO_USE_OPENGL)

include(CMakePackageConfigHelpers)

# Write package configuration file
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/mtao-config.cmake.in
  ${CMAKE_BINARY_DIR}/mtao-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mtao/cmake
)
set_property(TARGET mtao_headers PROPERTY EXPORT_NAME mtao::headers)
set_property(TARGET mtao_common PROPERTY EXPORT_NAME mtao::common)
set_property(TARGET mtao_geometry PROPERTY EXPORT_NAME mtao::geometry)
IF(MTAO_USE_OPENGL)
set_property(TARGET mtao_visualization PROPERTY EXPORT_NAME mtao::visualization)
ENDIF(MTAO_USE_OPENGL)
