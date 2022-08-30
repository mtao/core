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
add_library(partio_sequence_tools
    partio_sequence_tools/filter.cpp
    partio_sequence_tools/particles.cpp
    partio_sequence_tools/tube_mesh_constructor.cpp
    partio_sequence_tools/tube_mesh_constructor_gui.cpp
    )
if(MTAO_USE_OPENGL)
TARGET_LINK_LIBRARIES(partio_sequence_tools PUBLIC mtao::geometry
    mtao::visualization igl::core
    std::filesystem 
    PRIVATE igl::core tbb
    )
endif()

add_executable(partio_sequence_nearby_filter
    partio_sequence_nearby_filter.cpp)
target_link_libraries(partio_sequence_nearby_filter PRIVATE
    partio_sequence_tools tbb cxxopts igl::core)

if(MTAO_USE_OPENGL)
ADD_EXECUTABLE(partio_sequence_streamline_visualizer partio_sequence_streamline_visualizer.cpp
    partio_sequence_tools/viewer.cpp

    )
TARGET_LINK_LIBRARIES(partio_sequence_streamline_visualizer cxxopts
    mtao::visualization igl::core
    partio_sequence_tools)
endif()

endif()
if(MTAO_USE_JSON)
    add_executable(stopwatch_json2trace_event stopwatch_json2trace_event.cpp)
    TARGET_LINK_LIBRARIES(stopwatch_json2trace_event PRIVATE mtao::common cxxopts std::filesystem
    nlohmann_json::nlohmann_json)

endif()
#Half Iex IexMath IlmThread Imath tbb
