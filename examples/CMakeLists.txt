CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
IF(MTAO_USE_OPENGL)
ADD_SUBDIRECTORY(mesh_viewer)
ADD_SUBDIRECTORY(mesh2d_viewer)
ADD_SUBDIRECTORY(grid_viewer)
ADD_SUBDIRECTORY(grid2d_viewer)
ADD_SUBDIRECTORY(sphere_obj_generator)
ADD_SUBDIRECTORY(imgui_demo)
#ADD_SUBDIRECTORY(imgui_oldgl)
ADD_SUBDIRECTORY(enright_test)
ADD_SUBDIRECTORY(planar_mesh)
ADD_SUBDIRECTORY(picking2d)
ADD_SUBDIRECTORY(interpolation2d)
ENDIF(MTAO_USE_OPENGL)
#ADD_SUBDIRECTORY(sphere_obj_generator)
ADD_SUBDIRECTORY(obj2ply)
#ADD_SUBDIRECTORY(ply2obj)
