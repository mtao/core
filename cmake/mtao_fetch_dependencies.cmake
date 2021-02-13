include(FetchContent REQUIRED)

set(NLOHMANN_JSON_COMMIT
 v3.9.1
    )
set(PYBIND_COMMIT v2.6.1)
set(PARTIO_COMMIT v1.14.0)
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
    fetch_dep(eigen https://gitlab.com/libeigen/eigen.git 8bb0feba OFF)
    set(EIGEN3_INCLUDE_DIR ${eigen_SOURCE_DIR})
    find_package(Eigen3 REQUIRED)
ENDIF()
if(MTAO_USE_OPENGL)
    if(NOT MTAO_FORCE_LOCAL_GLFW)
        find_package(GLFW QUIET)
    endif()
    if(NOT GLFW_FOUND)
        fetch_dep(glfw https://github.com/glfw/glfw.git 0a49ef0a00baa3ab520ddc452f0e3b1e099c5589 ON)
        find_package(GLFW REQUIRED)
        set(GLFW_INCLUDE_DIR ${glfw_SOURCE_DIR})
        set(GLFW_INCLUDE_DIR ${glfw_SOURCE_DIR} CACHE STRING "The location of hte GLFW headers")
        set(GLFW_LIBRARY ${glfw_BINARY_DIR} CACHE STRING "The location of hte GLFW library")

        MESSAGE(STATUS "GLFW SOURCE DIR: ${glfw_SOURCE_DIR}")
    endif()
    if(NOT DEFINED IMGUI_DIR)
        fetch_dep(imgui https://github.com/ocornut/imgui.git v1.74 OFF)
        set(IMGUI_DIR ${imgui_SOURCE_DIR})
    endif()
    find_package(ImGui COMPONENTS Sources SourcesMiscCpp REQUIRED)
    set(ImGui_INCLUDE_DIR ${ImGui_INCLUDE_DIR} CACHE STRING "Location of the imgui library headers")
    if(NOT DEFINED COLORMAP_SHADERS_INCLUDE_DIR)
        fetch_dep(colormap_shaders https://github.com/kbinani/colormap-shaders.git 7ddaa34978c4729cfe6eddaf7bf68bb4831760bb OFF)
        set(COLORMAP_SHADERS_INCLUDE_DIR ${colormap_shaders_SOURCE_DIR}/include CACHE STRING "Colormap-shaders incldue dir")
        set(COLORMAP_SHADERS_SHADER_DIR ${colormap_shaders_SOURCE_DIR}/shaders CACHE STRING "colormap-shaders shader source dir")
    endif()
endif()


if(NOT DEFINED TRIANGLE_DIR)
    fetch_dep(triangle https://github.com/libigl/triangle.git d284c4a843efac043c310f5fa640b17cf7d96170 ON)
    set(triangle_INCLUDE_DIR ${triangle_SOURCE_DIR} CACHE STRING "The location of hte triangle.h header")

            hide_dependency_warnings(triangle)
endif()

if(MTAO_USE_ELTOPO)
    fetch_dep(eltopo https://github.com/mtao/eltopo.git 882d1f532ac11a2dae3dce90d39190a1ef451666 ON)
            hide_dependency_warnings(eltopo)
            hide_dependency_warnings(eltopo_common)
endif()
if(MTAO_USE_LOSTOPOS)
    fetch_dep(lostopos https://github.com/mtao/LosTopos.git 577636bfff2d72acf264c631610b345b61d797c2 ON)
endif()

if(MTAO_USE_OPENGL)
    if(NOT Corrade_FOUND)
        fetch_dep(corrade https://github.com/mtao/corrade configuration_reflection ON)
        #hide_dependency_warnings(Corrade )
    endif()
    if(NOT Magnum_FOUND)
        option(WITH_GLFWAPPLICATION "Build GlfwApplication library" ON)
        fetch_dep(magnum https://github.com/mosra/magnum v2020.06 ON)
        hide_dependency_warnings(Magnum)
        hide_dependency_warnings(MagnumMeshTools)
        hide_dependency_warnings(MagnumPlatformObjects)
        if(WITH_GLFWAPPLICATION)
            hide_dependency_warnings(MagnumGlfwApplication)
        endif()
    endif()
    if(NOT MagnumIntegration_FOUND)
        fetch_dep(magnum-integration https://github.com/mosra/magnum-integration v2020.06 ON)
        if(WITH_IMGUI) 
            hide_dependency_warnings(MagnumImGuiIntegration)
        endif()
    endif()

    if(NOT MagnumPlugins_FOUND)
        fetch_dep(magnum-plugins https://github.com/mosra/magnum-plugins v2020.06 ON)
    endif()
endif()
find_package(fmt 7.1.3 QUIET)
if(NOT fmt_FOUND)
fetch_dep(fmt https://github.com/fmtlib/fmt.git 7.1.3 ON)
endif()

find_package(range-v3 QUIET)
if(NOT range-v3_FOUND)
fetch_dep(range-v3 https://github.com/ericniebler/range-v3.git 0.10.0 ON)
endif()

find_package(spdlog 1.8.2 QUIET)
if(NOT spdlog_FOUND)
fetch_dep(spdlog https://github.com/gabime/spdlog.git v1.8.2 ON)
endif()

if(MTAO_BUILD_TESTING)
    if(NOT Catch2_FOUND)
        fetch_dep(
            catch2
            https://github.com/catchorg/Catch2.git
            v2.9.1
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
    find_package(openvdb 7.0.0 QUIET)
    if(NOT openvdb_FOUND)
        fetch_dep(openvdb https://github.com/AcademySoftwareFoundation/openvdb v7.0.0 ON)
    endif()
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
        fetch_dep(partio https://github.com/wdas/partio.git ${PARTIO_COMMIT}
            ON)
    endif()
endif()
