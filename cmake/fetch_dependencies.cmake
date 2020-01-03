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
endfunction()


if(MTAO_USE_OPENGL)
    if(NOT DEFINED ${IMGUI_DIR})
        fetch_dep(imgui https://github.com/ocornut/imgui.git v1.74 OFF)
        set(IMGUI_DIR ${imgui_SOURCE_DIR} CACHE STRING "the location of the imgui library")
    endif()
endif()

if(NOT DEFINED ${TRIANGLE_DIR})
    fetch_dep(triangle https://github.com/libigl/triangle.git d284c4a843efac043c310f5fa640b17cf7d96170 OFF)
    set(TRIANGLE_DIR ${triangle_SOURCE_DIR} CACHE STRING "The location of hte triangle library")
endif()

if(MTAO_USE_LOSTOPOS)
    fetch_dep(eltopo https://github.com/mtao/eltopo.git 8edd0090e8bcf4d3cfeb54a2acac500487d4463a ON)
endif()
if(MTAO_USE_LOSTOPOS)
    fetch_dep(lostopos https://github.com/mtao/LosTopos.git 577636bfff2d72acf264c631610b345b61d797c2 ON)
endif()
