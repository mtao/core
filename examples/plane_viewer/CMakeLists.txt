CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(plane_viewer
    main.cpp
    )
ADD_DEFINITIONS(-g)

TARGET_LINK_LIBRARIES(plane_viewer mtao::visualization mtao::geometry Magnum::MeshTools)
