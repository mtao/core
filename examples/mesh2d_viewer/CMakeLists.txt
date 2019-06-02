CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(mesh2d_viewer
    main.cpp
    )
ADD_DEFINITIONS(-g)

TARGET_LINK_LIBRARIES(mesh2d_viewer mtao_opengl mtao_geometry Magnum::MeshTools)
