CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(mesh_viewer
    main.cpp
    mesh.h
    mesh.cpp)
ADD_DEFINITIONS(-g)

TARGET_LINK_LIBRARIES(mesh_viewer mtao_opengl)
