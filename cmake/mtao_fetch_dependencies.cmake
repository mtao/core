include(FetchContent REQUIRED)

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


if(MTAO_USE_OPENGL)
    if(NOT DEFINED ${IMGUI_DIR})
        fetch_dep(imgui https://github.com/ocornut/imgui.git v1.74 OFF)
        set(IMGUI_DIR ${imgui_SOURCE_DIR})
        MESSAGE(STATUS "IMGUI DIR: ${IMGUI_DIR}")
    endif()
    find_package(ImGui REQUIRED)
endif()

if(NOT DEFINED ${TRIANGLE_DIR})
    fetch_dep(triangle https://github.com/libigl/triangle.git d284c4a843efac043c310f5fa640b17cf7d96170 ON)
    set(triangle_INCLUDE_DIR ${triangle_SOURCE_DIR} CACHE STRING "The location of hte triangle.h header")
endif()

if(MTAO_USE_ELTOPO)
    fetch_dep(eltopo https://github.com/mtao/eltopo.git a242873acff7a3ed6f5b2af9ed0001b661258ec7 ON)
endif()
if(MTAO_USE_LOSTOPOS)
    fetch_dep(lostopos https://github.com/mtao/LosTopos.git 577636bfff2d72acf264c631610b345b61d797c2 ON)
endif()

if(MTAO_USE_OPENGL)
    if(NOT Corrade_FOUND)
        fetch_dep(corrade https://github.com/mosra/corrade v2019.10 ON)
    endif()
    if(NOT Magnum_FOUND)
        option(WITH_GLFWAPPLICATION "Build GlfwApplication library" ON)
        fetch_dep(magnum https://github.com/mosra/magnum v2019.10 ON)
    endif()
    if(NOT MagnumIntegration_FOUND)
        option(WITH_EIGEN "Build EigenIntegration library" ON)
        option(WITH_IMGUI "Build ImGuiIntegration library" ON)
        fetch_dep(magnum-integration https://github.com/mosra/magnum-integration v2019.10 ON)
    endif()
endif()

if(BUILD_TESTING)
    if(NOT Catch2_FOUND)
        fetch_dep(
            catch2
            https://github.com/catchorg/Catch2.git
            v2.9.1
            ON
            )

    endif()
endif()
