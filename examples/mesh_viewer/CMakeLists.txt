
ADD_EXECUTABLE(mesh_viewer
    main.cpp
    mesh.h
    mesh.cpp)

TARGET_LINK_LIBRARIES(mesh_viewer mtao_opengl)
