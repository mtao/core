CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(picking2d
    main.cpp
    )
ADD_DEFINITIONS(-g)

TARGET_LINK_LIBRARIES(picking2d mtao::visualization)
