CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(grid2d_viewer
    main.cpp
    )
ADD_DEFINITIONS(-g)

TARGET_LINK_LIBRARIES(grid2d_viewer mtao::visualization)
