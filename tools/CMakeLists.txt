ADD_EXECUTABLE(xyz_to_arrows xyz_to_arrows.cpp)
TARGET_LINK_LIBRARIES(xyz_to_arrows mtao::geometry)
ADD_EXECUTABLE(xyz_to_balls xyz_to_balls.cpp)
TARGET_LINK_LIBRARIES(xyz_to_balls mtao::geometry)
ADD_EXECUTABLE(mesh_info mesh_info.cpp)
TARGET_LINK_LIBRARIES(mesh_info mtao::geometry nlohmann_json::nlohmann_json
    cxxopts)

ADD_EXECUTABLE(add_boundary_to_obj add_boundary_to_obj.cpp)
TARGET_LINK_LIBRARIES(add_boundary_to_obj mtao::geometry nlohmann_json::nlohmann_json
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

if(MTAO_USE_PARTIO)
ADD_EXECUTABLE(partio_sequence_to_streamlines partio_sequence_to_streamlines.cpp)
TARGET_LINK_LIBRARIES(partio_sequence_to_streamlines mtao::geometry cxxopts )

ADD_EXECUTABLE(partio_to_obj partio_to_obj.cpp)
TARGET_LINK_LIBRARIES(partio_to_obj mtao::geometry cxxopts )

ADD_EXECUTABLE(partio_to_obj_filter partio_to_obj_filter.cpp)
TARGET_LINK_LIBRARIES(partio_to_obj_filter mtao::geometry cxxopts )

ADD_EXECUTABLE(partio_to_arrow_obj partio_to_arrow_obj.cpp)
TARGET_LINK_LIBRARIES(partio_to_arrow_obj mtao::geometry cxxopts tbb)
ADD_EXECUTABLE(xyz_to_partio xyz_to_partio.cpp)
TARGET_LINK_LIBRARIES(xyz_to_partio mtao::geometry cxxopts)

ADD_EXECUTABLE(partio_to_ply partio_to_ply.cpp)
TARGET_LINK_LIBRARIES(partio_to_ply mtao::geometry cxxopts tbb)

if(MTAO_USE_OPENVDB)
ADD_EXECUTABLE(partio_to_vdb_points partio_to_vdb_points.cpp)
TARGET_LINK_LIBRARIES(partio_to_vdb_points mtao::geometry cxxopts openvdb)
endif()

if(MTAO_USE_OPENGL)
ADD_EXECUTABLE(partio_sequence_streamline_visualizer partio_sequence_streamline_visualizer.cpp)
TARGET_LINK_LIBRARIES(partio_sequence_streamline_visualizer mtao::geometry cxxopts mtao::visualization std::filesystem)
endif()
endif()
#Half Iex IexMath IlmThread Imath tbb
