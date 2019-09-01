CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
ADD_EXECUTABLE(planar_mesh
    src/main.cpp
    src/geometry.cpp
    src/example_boundaries.cpp
    include/geometry.h
    include/example_boundaries.h
    )

ADD_DEFINITIONS(-g)
TARGET_LINK_LIBRARIES(planar_mesh mtao::visualization mtao::geometry Magnum::MeshTools)
