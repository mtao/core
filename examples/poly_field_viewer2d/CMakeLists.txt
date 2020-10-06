CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(poly_field_viewer2d
    main.cpp
    )
ADD_DEFINITIONS(-g)

TARGET_LINK_LIBRARIES(poly_field_viewer2d mtao::visualization mtao::geometry Magnum::MeshTools)
