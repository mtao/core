CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(mesh_viewer
    main.cpp)
ADD_DEFINITIONS(-g)

TARGET_LINK_LIBRARIES(mesh_viewer mtao::visualization mtao::geometry Magnum::MeshTools)

#target_link_libraries(MyApplication PRIVATE
#    Magnum::Application
#    Magnum::GL
#    Magnum::Magnum)
