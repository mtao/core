CMAKE_MINIMUM_REQUIRED(VERSION 3.13.4)
PROJECT(Mtao)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_NO_SYSTEM_FROM_IMPORTED ON)

SET(MTAO_BUILD_TESTING ${BUILD_TESTING})

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()


# 
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY AND NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY AND NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "" FORCE)
endif()

SET(CMAKE_RUNTIME_OUTPUT_DIR ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIR ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIR ${PROJECT_BINARY_DIR}/lib)

SET(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
IF(CMAKE_BUILD_TYPE MATCHES Debug)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
ADD_DEFINITIONS(-Wall)
#ADD_DEFINITIONS(-Wno-deprecated-declarations)
#ADD_DEFINITIONS(-Wno-unused-variable)


OPTION(MTAO_HIDE_DEPENDENCY_WARNINGS "Hides build warnings from dependent
libraries" ON)
OPTION(MTAO_USE_ELTOPO "Should we build the el topo submodule" ON)
OPTION(MTAO_USE_LOSTOPOS "Should we build the los topos submodule" OFF)
OPTION(MTAO_USE_OPENGL "Build opengl stuff" ON)
OPTION(MTAO_USE_PYTHON "Should build python library" ON)
# Temporary fix for having wayland support in glfw (https://github.com/glfw/glfw/issues/1655)
OPTION(MTAO_FORCE_LOCAL_GLFW "Build our own version of GLFW" OFF)
OPTION(MTAO_USE_OPENVDB "Build openvdb stuff" OFF)
OPTION(MTAO_USE_PNGPP "Use PNG++ for screenshots" ON)
OPTION(MTAO_USE_JSON "Use json" ON)
OPTION(MTAO_USE_PARTIO "Use partio" ON)
OPTION(MTAO_USE_IGL "Use libigl" ON)
OPTION(MTAO_BUILD_STATIC "Build static libraries" ON)

# https://medium.com/@alasher/colored-c-compiler-output-with-ninja-clang-gcc-10bfe7f2b949
option (MTAO_FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only and affects global compile_options)." OFF)
if (MTAO_FORCE_COLORED_OUTPUT)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif()
include(mtao_fetch_dependencies)

FIND_PACKAGE(Eigen3 3.3.3 REQUIRED)




if(MTAO_BUILD_STATIC)
    set(MTAO_LIBRARY_TYPE STATIC)
else()
    set(MTAO_LIBRARY_TYPE SHARED)
endif()

# Include dirs

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Filesystem REQUIRED)

FIND_PACKAGE(OpenMP REQUIRED)
SET(OpenMP_TARGET OpenMP::OpenMP_CXX)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    message(STATUS "PkgConfigFound")
    pkg_check_modules(tbb QUIET IMPORTED_TARGET tbb)
    if(TARGET PkgConfig::tbb)
        message(STATUS "TBB Target found by pkgconfig")
        SET(MTAO_TBB_AVAILABLE ON)
    endif()
endif()
OPTION(MTAO_USE_TBB "Use TBB for screenshots" ${MTAO_TBB_AVAILABLE})

if((MTAO_USE_TBB) AND NOT (MTAO_TBB_AVAILABLE))
    message(SEND_ERROR "TBB REQUESTED BUT NOT AVAIALBLE")
endif()

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
TARGET_LINK_LIBRARIES(mtao_headers PUBLIC INTERFACE Eigen3::Eigen range-v3)


set(MTAO_COMMON_SRCS

    include/mtao/logging/logger.hpp
    include/mtao/logging/timer.hpp
    include/mtao/logging/profiler.hpp
    include/mtao/cmdline_parser.hpp
    src/logging/logger.cpp
    src/logging/timer.cpp
    src/logging/profiler.cpp
    src/cmdline_parser.cpp
    src/algebra/pascal_triangle.cpp
    src/utils/get_relative_path.cpp
    )

if(MTAO_USE_JSON)
    set(MTAO_COMMON_SRCS ${MTAO_COMMON_SRCS}
        src/utils/corrade_configuration_json_interchange.cpp)
endif()
ADD_LIBRARY(mtao_common ${MTAO_LIBRARY_TYPE}
    ${MTAO_COMMON_SRCS}
    )

set_property(TARGET mtao_common PROPERTY CXX_STANDARD 20)

TARGET_LINK_LIBRARIES(mtao_common PUBLIC mtao_headers Eigen3::Eigen
    spdlog::spdlog fmt Corrade::Utility std::filesystem
    nlohmann_json::nlohmann_json)

if(MTAO_USE_TBB)
    TARGET_LINK_LIBRARIES(mtao_headers INTERFACE PkgConfig::tbb)
    target_compile_definitions(mtao_common PUBLIC -DMTAO_TBB_ENABLED)
endif()

IF(MTAO_USE_OPENGL)
    CORRADE_ADD_RESOURCE(MtaoShaders ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/shaders/resources.conf)

ADD_LIBRARY(mtao_visualization ${MTAO_LIBRARY_TYPE}
    ${MtaoShaders}
    ${COMMON_HEADERS}
    src/opengl/Window.cpp
    src/opengl/Window2.cpp
    src/opengl/Window3.cpp
    #src/opengl/Monitor.cpp
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
    src/opengl/objects/plane.cpp
    src/opengl/objects/sphere.cpp
    src/opengl/shaders.cpp
    src/opengl/drawables.cpp
    src/visualization/imgui/interval_scaler_settings_widget.cpp
    src/visualization/imgui/utils.cpp
    src/visualization/interval_scaler_settings.cpp
    src/visualization/imgui/colormap_settings_widget.cpp


    #Shaders
    src/opengl/shaders/vector_field.cpp
    src/opengl/shaders/polynomial_scalar_field.cpp
    src/opengl/shaders/shader_resources.cpp
    )
set_property(TARGET mtao_visualization PROPERTY CXX_STANDARD 20)
if(MTAO_USE_PNGPP)
    FIND_PACKAGE(png++ REQUIRED)
    TARGET_COMPILE_DEFINITIONS(mtao_visualization PRIVATE -DMTAO_HAS_LIBPNGPP)
    TARGET_INCLUDE_DIRECTORIES(mtao_visualization PRIVATE ${png++_INCLUDE_DIRS})

    #target_link_libraries(mtao_visualization PRIVATE
    #    MagnumPlugins::PngImageConverter)
endif(MTAO_USE_PNGPP)

TARGET_INCLUDE_DIRECTORIES(mtao_visualization PUBLIC ${COLORMAP_SHADERS_INCLUDE_DIR})

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
    PRIVATE ImGui::SourcesMiscCpp
    PRIVATE OpenGL::GL 
    GLFW::GLFW 
    dl 
    ${png++_LIBRARIES} 
    Corrade::PluginManager
    mtao::geometry
    )
if(WITH_PNGIMAGECONVERTER)
    TARGET_COMPILE_DEFINITIONS(mtao_visualization PRIVATE
        -DMTAO_HAS_MAGNUM_PNGIMAGECONVERTER)
    target_link_libraries(mtao_visualization PRIVATE
    MagnumPlugins::PngImageConverter
    )
endif()
ENDIF(MTAO_USE_OPENGL)



SET(GEOMETRY_SRCS
    src/geometry/mesh/shapes/cube.cpp
    src/geometry/mesh/shapes/tube.cpp
    src/geometry/mesh/halfedge.cpp
    src/geometry/mesh/halfedge_fv_map.cpp
    src/geometry/mesh/read_obj.cpp
    src/geometry/point_cloud/read_xyz.cpp
    src/geometry/mesh/write_obj.cpp
    src/geometry/mesh/boundary_elements.cpp
    #src/geometry/mesh/read_ply.cpp
    src/geometry/mesh/write_ply.cpp
    src/geometry/mesh/triangle/triangle_wrapper.cpp
    src/geometry/mesh/triangle/mesh.cpp
    #src/geometry/mesh/read_plc.cpp
    include/mtao/geometry/mesh/halfedge.hpp
    include/mtao/geometry/mesh/halfedge_fv_map.hpp
    include/mtao/geometry/mesh/read_obj.hpp
    src/geometry/mesh/ply.cpp
    src/geometry/mesh/polygon_boundary_triangulation.cpp
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
IF(MTAO_USE_PARTIO)
    SET(GEOMETRY_SRCS ${GEOMETRY_SRCS}
        src/geometry/point_cloud/partio_loader.cpp)
ENDIF(MTAO_USE_PARTIO)

ADD_LIBRARY(mtao_geometry ${MTAO_LIBRARY_TYPE}
    ${GEOMETRY_SRCS}
    )
set_property(TARGET mtao_geometry PROPERTY CXX_STANDARD 20)



if(MTAO_USE_PYTHON)
add_library(mtao_python
    src/python/load_python_function.cpp
    )

target_link_libraries(mtao_python PUBLIC pybind11::embed mtao::common)
set_property(TARGET mtao_python PROPERTY CXX_STANDARD 20)
#set_target_properties(mtao_python PROPERTIES CXX_VISIBILITY_PRESET hidden)
endif()



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

IF(MTAO_USE_PARTIO)
    target_link_libraries(mtao_geometry PUBLIC partio)
endif(MTAO_USE_PARTIO)


#SET(MTAO_EXT_LIBS mtao::common mtao::opengl mtao::geometry)
#SET(MTAO_EXT_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include)

ADD_SUBDIRECTORY(tools EXCLUDE_FROM_ALL)

MESSAGE(STATUS ${MTAO_BUILD_TESTING})
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND MTAO_BUILD_TESTING)
    ADD_SUBDIRECTORY(tests)
endif()
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
ADD_SUBDIRECTORY(examples EXCLUDE_FROM_ALL)
endif()

add_library(mtao::headers ALIAS mtao_headers)
add_library(mtao::common ALIAS mtao_common)
add_library(mtao::geometry ALIAS mtao_geometry)
if(MTAO_USE_PYTHON)
add_library(mtao::python ALIAS mtao_python)
endif()
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
