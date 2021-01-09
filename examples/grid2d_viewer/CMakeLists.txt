CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(grid2d_viewer
    main.cpp
    )
ADD_DEFINITIONS(-g)

TARGET_LINK_LIBRARIES(grid2d_viewer mtao::visualization)
if(MTAO_USE_PYTHON)
TARGET_LINK_LIBRARIES(grid2d_viewer mtao::python)
target_compile_definitions(grid2d_viewer PRIVATE ENABLE_PYTHON_GUI)

endif()
