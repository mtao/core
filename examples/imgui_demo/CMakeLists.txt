CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(imgui_demo
    main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../extern/imgui/imgui_demo.cpp
    )
ADD_DEFINITIONS(-g)
    find_package(MagnumIntegration COMPONENTS ImGui)
    find_package(ImGui REQUIRED)

    TARGET_LINK_LIBRARIES(imgui_demo mtao::visualization ImGui::ImGui
    MagnumIntegration::ImGui
    )
