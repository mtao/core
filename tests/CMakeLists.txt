INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

FUNCTION(ADD_MYTEST BINNAME)
    ADD_EXECUTABLE(${BINNAME} ${ARGN})
    TARGET_LINK_LIBRARIES(${BINNAME} mtao::common)
ENDFUNCTION(ADD_MYTEST)

ADD_DEFINITIONS(-g)
ADD_MYTEST(grid_test grid_test.cpp)
ADD_MYTEST(grid_data_test grid_data_test.cpp)
ADD_MYTEST(angle_test angle_test.cpp)
ADD_MYTEST(levelset_test levelset_test.cpp)
ADD_MYTEST(multi_loop_test multi_loop_test.cpp)
ADD_MYTEST(static_type_iteration_test static_type_iteration_test.cpp)
ADD_MYTEST(eigen_interweave_test eigen_interweave_test.cpp)
ADD_MYTEST(eigen_stack_test eigen_stack_test.cpp)
ADD_MYTEST(volume_test volume_test.cpp)
ADD_MYTEST(boundary_test boundary_test.cpp)
ADD_MYTEST(graph_boundary_test graph_boundary_test.cpp)
ADD_MYTEST(sphere_test sphere_test.cpp)
ADD_MYTEST(area_normals_test area_normals_test.cpp)
ADD_MYTEST(mean_curvature_normals_test mean_curvature_normals_test.cpp)
ADD_MYTEST(triangulation_test triangulation_test.cpp)
ADD_MYTEST(vertex_normals_test vertex_normals_test.cpp)
ADD_MYTEST(type_size_test type_size_test.cpp)
#ADD_MYTEST(copyable_unique_ptr_test copyable_unique_ptr_test.cpp)
ADD_MYTEST(enumerate_test enumerate_test.cpp)
ADD_MYTEST(range_test range_test.cpp)
ADD_MYTEST(zip_test zip_test.cpp)
ADD_MYTEST(reverse_test reverse_test.cpp)
ADD_MYTEST(cycle_test cycle_test.cpp)
ADD_MYTEST(iterator_interval_test iterator_interval_test.cpp)
ADD_MYTEST(element_filter_test element_filter_test.cpp)
ADD_MYTEST(slice_filter_test slice_filter_test.cpp)
ADD_MYTEST(barycentric_test barycentric_test.cpp)
ADD_MYTEST(barycentric_matrix_test barycentric_matrix_test.cpp)
#ADD_MYTEST(spherical_mean_value_test spherical_mean_value_test.cpp)
ADD_MYTEST(solid_angle_test solid_angle_test.cpp)
ADD_MYTEST(mesh_bary_test mesh_bary_test.cpp)
ADD_MYTEST(kdtree_test kdtree_test.cpp)
ADD_MYTEST(vertex_pruning_test vertex_pruning_test.cpp)
ADD_MYTEST(opposite_volumes_test opposite_volumes_test.cpp)
ADD_MYTEST(cot_laplacian_test cot_laplacian_test.cpp)

ADD_MYTEST(earclipping_with_input_test earclipping_with_input.cpp)
ADD_MYTEST(winding_plus_earclipping_test winding_plus_earclipping_test.cpp)
ADD_MYTEST(colvector_loop_test colvector_loop_test.cpp)
ADD_MYTEST(reindexer_test reindexer_test.cpp)

ADD_MYTEST(stl2eigen_test stl2eigen_test.cpp)

ADD_MYTEST(bounding_box_test bounding_box_test.cpp)

ADD_MYTEST(tube_creation_test tube_creation_test.cpp)

ADD_MYTEST(horner_evaluation_test horner_evaluation_test.cpp)
ADD_MYTEST(log_test log_test.cpp)
ADD_MYTEST(profiler_test profiler_test.cpp)

ADD_MYTEST(tuple_call_test tuple_call_test.cpp)

ADD_EXECUTABLE(halfedge_test halfedge_test.cpp)
TARGET_LINK_LIBRARIES(halfedge_test mtao::geometry)
ADD_EXECUTABLE(halfedge_concentric_circle_test halfedge_concentric_circle_test.cpp)
TARGET_LINK_LIBRARIES(halfedge_concentric_circle_test  mtao::geometry)
ADD_EXECUTABLE(strand_test strand_test.cpp)
TARGET_LINK_LIBRARIES(strand_test mtao::geometry)
ADD_EXECUTABLE(halfedge_topology_test halfedge_topology_test.cpp)
TARGET_LINK_LIBRARIES(halfedge_topology_test mtao::geometry)
ADD_EXECUTABLE(halfedge_submesh_test halfedge_submesh_test.cpp)
TARGET_LINK_LIBRARIES(halfedge_submesh_test mtao::geometry)
ADD_EXECUTABLE(embedded_halfedge_test embedded_halfedge_test.cpp)
TARGET_LINK_LIBRARIES(embedded_halfedge_test mtao::geometry)

#ADD_EXECUTABLE(static_iteration_test src/static_iteration_test.cpp)
ADD_MYTEST(combination_test combination_test.cpp)
ADD_MYTEST(staggered_grid_test staggered_grid_test.cpp)
ADD_MYTEST(staggered_grid_form_test staggered_grid_form_test.cpp)
ADD_MYTEST(staggered_grid_cell_test staggered_grid_cell_test.cpp)
ADD_MYTEST(staggered_grid_boundary_test staggered_grid_boundary_test.cpp)
ADD_MYTEST(staggered_grid_util_test staggered_grid_util_test.cpp)
ADD_MYTEST(laplacian_smoothing_test laplacian_smoothing_test.cpp)

ADD_MYTEST(disjoint_set_test disjoint_set_test.cpp)
ADD_MYTEST(static_loop_test static_loop_test.cpp)

ADD_MYTEST(cltest cltest.cpp)

ADD_MYTEST(hotkey_test hotkey_test.cpp)


#ADD_EXECUTABLE(particle_sampling_test src/particle_sampling_test.cpp ${CORE_INCLUDE}/grid.h ${CORE_INCLUDE}/grid_utils.h
#    ${CORE_INCLUDE}/arguments.h ${CORE_INCLUDE}/particle_sampler.h)
#
#
#ADD_EXECUTABLE(parsing_test src/parsing_test.cpp ${CORE_INCLUDE}/grid.h ${CORE_INCLUDE}/grid_utils.h
#    ${CORE_INCLUDE}/arguments.h src/parsing_test.cpp src/data_parser.cpp
#    src/coloring.cpp
#    src/scalar_function_examples.cpp
#    ${CORE_INCLUDE}/data_parser.h)
#
#ADD_EXECUTABLE(func_print_test src/function_print_test.cpp ${CORE_INCLUDE}/grid.h ${CORE_INCLUDE}/grid_utils.h
#    src/scalar_function_examples.cpp
#    src/coloring.cpp
#    ${CORE_INCLUDE}/coloring.h
#    ${CORE_INCLUDE}/discrete_scalar_function.h ${CORE_INCLUDE}/scalar_function.h
#    ${CORE_INCLUDE}/scalar_function_examples.h
#    ${CORE_INCLUDE}/arguments.h)
#
#
#
#ADD_EXECUTABLE(advection_test src/advection_test.cpp include/grid.h include/grid_utils.h
#    src/scalar_function_examples.cpp
#    include/discrete_scalar_function.h include/scalar_function.h
#    include/scalar_function_examples.h
#    include/arguments.h)
#
#TARGET_LINK_LIBRARIES(advection_test pthread)
