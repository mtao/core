CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(imgui_oldgl
    main.cpp
    )
ADD_DEFINITIONS(-g)

TARGET_LINK_LIBRARIES(imgui_oldgl mtao_opengl)
