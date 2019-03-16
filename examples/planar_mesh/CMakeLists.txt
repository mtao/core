CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
ADD_EXECUTABLE(tw
    src/main.cpp
    src/geometry.cpp
    src/example_boundaries.cpp
    include/geometry.h
    include/example_boundaries.h
    )

TARGET_LINK_LIBRARIES(planar_mesh mtao_opengl mtao_geometry)
