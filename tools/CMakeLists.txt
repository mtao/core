ADD_EXECUTABLE(xyz_to_arrows xyz_to_arrows.cpp)
TARGET_LINK_LIBRARIES(xyz_to_arrows mtao::geometry)
ADD_EXECUTABLE(mesh_info mesh_info.cpp)
TARGET_LINK_LIBRARIES(mesh_info mtao::geometry)
