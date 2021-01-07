ADD_EXECUTABLE(xyz_to_arrows xyz_to_arrows.cpp)
TARGET_LINK_LIBRARIES(xyz_to_arrows mtao::geometry)
ADD_EXECUTABLE(xyz_to_balls xyz_to_balls.cpp)
TARGET_LINK_LIBRARIES(xyz_to_balls mtao::geometry)
ADD_EXECUTABLE(mesh_info mesh_info.cpp)
TARGET_LINK_LIBRARIES(mesh_info mtao::geometry nlohmann_json::nlohmann_json
    cxxopts)

ADD_EXECUTABLE(json_to_corrade_configuration json_to_corrade_configuration.cpp)
TARGET_LINK_LIBRARIES(json_to_corrade_configuration mtao::common cxxopts)

ADD_EXECUTABLE(corrade_configuration_to_json corrade_configuration_to_json.cpp)
TARGET_LINK_LIBRARIES(corrade_configuration_to_json mtao::common cxxopts)

if(MTAO_USE_OPENVDB)
ADD_EXECUTABLE(vdb_to_mesh vdb_to_mesh.cpp)
TARGET_LINK_LIBRARIES(vdb_to_mesh mtao::geometry cxxopts openvdb)
ADD_EXECUTABLE(mesh_to_vdb mesh_to_vdb.cpp)
TARGET_LINK_LIBRARIES(mesh_to_vdb mtao::geometry cxxopts openvdb)
endif()
#Half Iex IexMath IlmThread Imath tbb
