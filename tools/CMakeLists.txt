ADD_EXECUTABLE(xyz_to_arrows xyz_to_arrows.cpp)
TARGET_LINK_LIBRARIES(xyz_to_arrows mtao::geometry)
ADD_EXECUTABLE(xyz_to_balls xyz_to_balls.cpp)
TARGET_LINK_LIBRARIES(xyz_to_balls mtao::geometry)
ADD_EXECUTABLE(mesh_info mesh_info.cpp)
TARGET_LINK_LIBRARIES(mesh_info mtao::geometry)

if(MTAO_USE_OPENVDB)
ADD_EXECUTABLE(vdb_to_mesh vdb_to_mesh.cpp)
TARGET_LINK_LIBRARIES(vdb_to_mesh mtao::geometry cxxopts openvdb)
endif()
#Half Iex IexMath IlmThread Imath tbb
