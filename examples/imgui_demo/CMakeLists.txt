CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(imgui_demo
    main.cpp
    )
ADD_DEFINITIONS(-g)

    TARGET_LINK_LIBRARIES(imgui_demo mtao::visualization ImGui::ImGui
    MagnumIntegration::ImGui
    )
