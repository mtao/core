include(FetchContent REQUIRED)

include(mtao_dependency_versions)
# MAGNUM SETTINGS
option(BUILD_TESTS "Build tests (for mosra libs)" OFF)
option(BUILD_TESTSUITE "Build test suite library (mosra)" OFF)
option(BUILD_PLUGINS_STATIC "Build Plugins statically " ON)
option(WITH_PNGIMAGECONVERTER "Build PngImageConverter plugin" ${MTAO_USE_PNGPP})
# Magnum integration options
option(WITH_EIGEN "MagnumIntegration Build EigenIntegration library" ON)
option(WITH_IMGUI "MagnumIntegration Build ImGuiIntegration library" ON)

option(PYILMBASE_ENABLE "Enable python bindings to pyilm" OFF)

#MESSAGE(STATUS "PNGIMAGECONVERTER IS ${WITH_PNGIMAGECONVERTER} because MTAO_USE_PNGPP is ${MTAO_USE_PNGPP}")
function(fetch_dep REPO_NAME GIT_REPO GIT_TAG ADD_SUBDIR)
    FetchContent_Declare(
        ${REPO_NAME}
        GIT_REPOSITORY ${GIT_REPO}
        #GIT_TAG f6b406427400ed7ddb56cfc2577b6af571827c8c
        GIT_TAG ${GIT_TAG}
        )
    if(ADD_SUBDIR)
        if(${CMAKE_VERSION} VERSION_LESS 3.14)
            FetchContent_Populate(${REPO_NAME})
            add_subdirectory(${${REPO_NAME}_SOURCE_DIR} ${${REPO_NAME}_BINARY_DIR})
        else()
            FetchContent_MakeAvailable(${REPO_NAME})
        endif()
    else()
        FetchContent_Populate(${REPO_NAME})
    endif()
    set(${REPO_NAME}_SOURCE_DIR ${${REPO_NAME}_SOURCE_DIR} PARENT_SCOPE)
    set(${REPO_NAME}_BINARY_DIR ${${REPO_NAME}_BINARY_DIR} PARENT_SCOPE)
endfunction()


function(hide_dependency_warnings TARGET_NAME)
    if(MTAO_HIDE_DEPENDENCY_WARNINGS)
        target_compile_options(${TARGET_NAME} PRIVATE "-w")
    endif()
endfunction()

if(NOT Eigen3_FOUND)
    set(BUILD_TESTING OFF)
    fetch_dep(eigen https://gitlab.com/libeigen/eigen.git ${EIGEN_COMMIT} OFF)
    set(EIGEN3_INCLUDE_DIR ${eigen_SOURCE_DIR})
    find_package(Eigen3 REQUIRED)
ENDIF()
if(MTAO_USE_OPENGL)
    if(NOT MTAO_FORCE_LOCAL_GLFW)
        find_package(GLFW QUIET)
    endif()
    if(NOT GLFW_FOUND)
        fetch_dep(glfw https://github.com/glfw/glfw.git ${GLTF_COMMIT} ON)
        find_package(GLFW REQUIRED)
        set(GLFW_INCLUDE_DIR ${glfw_SOURCE_DIR})
        set(GLFW_INCLUDE_DIR ${glfw_SOURCE_DIR} CACHE STRING "The location of hte GLFW headers")
        set(GLFW_LIBRARY ${glfw_BINARY_DIR} CACHE STRING "The location of hte GLFW library")

        MESSAGE(STATUS "GLFW SOURCE DIR: ${glfw_SOURCE_DIR}")
    endif()
    if(NOT DEFINED IMGUI_DIR)
        fetch_dep(imgui https://github.com/ocornut/imgui.git ${IMGUI_COMMIT} OFF)
        set(IMGUI_DIR ${imgui_SOURCE_DIR})
    endif()
    find_package(ImGui COMPONENTS Sources SourcesMiscCpp REQUIRED)
    set(ImGui_INCLUDE_DIR ${ImGui_INCLUDE_DIR} CACHE STRING "Location of the imgui library headers")
    if(NOT DEFINED COLORMAP_SHADERS_INCLUDE_DIR)
        fetch_dep(colormap_shaders https://github.com/kbinani/colormap-shaders.git ${COLORMAP_SHADERS_COMMIT} OFF)
        set(COLORMAP_SHADERS_INCLUDE_DIR ${colormap_shaders_SOURCE_DIR}/include CACHE STRING "Colormap-shaders incldue dir")
        set(COLORMAP_SHADERS_SHADER_DIR ${colormap_shaders_SOURCE_DIR}/shaders CACHE STRING "colormap-shaders shader source dir")
    endif()
endif()


if(NOT DEFINED TRIANGLE_DIR)
    fetch_dep(triangle https://github.com/libigl/triangle.git ${TRIANGLE_COMMIT} ON)
    set(triangle_INCLUDE_DIR ${triangle_SOURCE_DIR} CACHE STRING "The location of hte triangle.h header")

            hide_dependency_warnings(triangle)
endif()

if(MTAO_USE_ELTOPO)
    fetch_dep(eltopo https://github.com/mtao/eltopo.git ${ELTOPO_COMMIT} ON)
            hide_dependency_warnings(eltopo)
            hide_dependency_warnings(eltopo_common)
endif()
if(MTAO_USE_LOSTOPOS)
    fetch_dep(lostopos https://github.com/mtao/LosTopos.git ${LOSTOPOS_COMMIT} ON)
endif()


    if(NOT Corrade_FOUND)
        fetch_dep(corrade https://github.com/mtao/corrade ${CORRADE_COMMIT} ON)
        #hide_dependency_warnings(Corrade )
    endif()
if(MTAO_USE_OPENGL)
    if(NOT Magnum_FOUND)
        option(WITH_GLFWAPPLICATION "Build GlfwApplication library" ON)
        fetch_dep(magnum https://github.com/mosra/magnum ${MAGNUM_COMMIT} ON)
        hide_dependency_warnings(Magnum)
        hide_dependency_warnings(MagnumMeshTools)
        hide_dependency_warnings(MagnumPlatformObjects)
        if(WITH_GLFWAPPLICATION)
            hide_dependency_warnings(MagnumGlfwApplication)
        endif()
    endif()
    if(NOT MagnumIntegration_FOUND)
        fetch_dep(magnum-integration https://github.com/mosra/magnum-integration ${MAGNUM_INTEGRATION_COMMIT} ON)
        if(WITH_IMGUI) 
            hide_dependency_warnings(MagnumImGuiIntegration)
        endif()
    endif()

    if(NOT MagnumPlugins_FOUND)
        fetch_dep(magnum-plugins https://github.com/mosra/magnum-plugins ${MAGNUM_PLUGINS_COMMIT} ON)
    endif()
endif()
find_package(fmt ${FMT_VERSION} QUIET)
if(NOT fmt_FOUND)
    fetch_dep(fmt https://github.com/fmtlib/fmt.git ${FMT_COMMIT} ON)
endif()

find_package(range-v3 QUIET)
if(NOT range-v3_FOUND)
    fetch_dep(range-v3 https://github.com/ericniebler/range-v3.git ${RANGE_V3_COMMIT} ON)
endif()

find_package(spdlog ${SPDLOG_VERSION} QUIET)
if(NOT spdlog_FOUND)
    set(SPDLOG_BUILD_SHARED ON)
    fetch_dep(spdlog https://github.com/gabime/spdlog.git ${SPDLOG_COMMIT} ON)
endif()

if(MTAO_BUILD_TESTING)
    if(NOT Catch2_FOUND)
        fetch_dep(
            catch2
            https://github.com/catchorg/Catch2.git
            ${CATCH2_COMMIT}
            ON
            )

    endif()
endif()


if(MTAO_USE_JSON)
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    # turn off the tests by default
option(JSON_BuildTests "Build the unit tests when BUILD_TESTING is enabled." OFF)
fetch_dep(nlohmann_json https://github.com/nlohmann/json.git 
    ${NLOHMANN_JSON_COMMIT}
    ON)
endif()
endif()

if(MTAO_USE_OPENVDB)
    #find_package(openexr 2.5.4 QUIET)
    #if(NOT openexr_FOUND)
    #    fetch_dep(openexr https://github.com/AcademySoftwareFoundation/openexr v2.5.4 ON)
    #endif()
    #find_package(openvdb ${OPENVDB_VERSION} QUIET)
    #if(NOT openvdb_FOUND)
    #fetch_dep(openvdb https://github.com/AcademySoftwareFoundation/openvdb v${OPENVDB_VERSION} ON)
        #endif()
        fetch_dep(openvdb https://github.com/AcademySoftwareFoundation/openvdb ${OPENVDB_COMMIT} ON)
endif()


option(CXXOPTS_BUILD_EXAMPLES "Set to ON to build cxxopts examples" OFF)
option(CXXOPTS_BUILD_TESTS "Set to ON to build cxxopts tests" OFF)
find_package(cxxopts 2.2.0 QUIET)
if(NOT cxxopts_FOUND)
    fetch_dep(cxxopts https://github.com/jarro2783/cxxopts v2.2.0 ON)
endif()

if(MTAO_USE_PYTHON)
    find_package(pybind11 QUIET)
    if(NOT pybind11_FOUND)
        fetch_dep(pybind11 https://github.com/pybind/pybind11.git ${PYBIND_COMMIT} ON)
    endif()
endif()

if(MTAO_USE_PARTIO)
    find_package(partio QUIET)
    if(NOT partio_FOUND)
        fetch_dep(partio https://github.com/mtao/partio.git ${PARTIO_COMMIT}
            ON)
    endif()
endif()

option(LIBIGL_USE_STATIC_LIBRARY "Use libigl as static library" OFF)
option(LIBIGL_WITH_COMISO            "Use CoMiso"                   OFF)
option(LIBIGL_WITH_EMBREE            "Use Embree"                   OFF)
option(LIBIGL_WITH_OPENGL            "Use OpenGL"                   OFF)
option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"                     OFF)
option(LIBIGL_WITH_OPENGL_GLFW_IMGUI "Use ImGui"                    OFF)
option(LIBIGL_WITH_PNG               "Use PNG"                      OFF)
option(LIBIGL_WITH_TETGEN            "Use Tetgen"                   OFF)
option(LIBIGL_WITH_TRIANGLE          "Use Triangle"                 OFF)
option(LIBIGL_WITH_PREDICATES        "Use exact predicates"         OFF)
option(LIBIGL_WITH_XML               "Use XML"                      OFF)
option(LIBIGL_WITH_PYTHOFF            "Use Python"                  OFF)
option(LIBIGL_SKIP_DOWNLOAD "Skip downloading external libraries" ON)
if(LIBIGL_PATH)
    ADD_SUBDIRECTORY("${LIBIGL_PATH}" ${CMAKE_BINARY_DIR}/libigl EXCLUDE_FROM_ALL)
else()
    fetch_dep(libigl https://github.com/mtao/libigl.git ${LIBIGL_COMMIT} ON)
endif()
