ADD_DEFINITIONS(-g)
ADD_EXECUTABLE(grid_test grid_test.cpp)
ADD_EXECUTABLE(grid_data_test grid_data_test.cpp)
ADD_EXECUTABLE(levelset_test levelset_test.cpp)
ADD_EXECUTABLE(multi_loop_test multi_loop_test.cpp)
ADD_EXECUTABLE(static_type_iteration_test static_type_iteration_test.cpp)
ADD_EXECUTABLE(eigen_interweave_test eigen_interweave_test.cpp)
ADD_EXECUTABLE(eigen_stack_test eigen_stack_test.cpp)
ADD_EXECUTABLE(volume_test volume_test.cpp)
ADD_EXECUTABLE(boundary_test boundary_test.cpp)
ADD_EXECUTABLE(sphere_test sphere_test.cpp)
ADD_EXECUTABLE(area_normals_test area_normals_test.cpp)
ADD_EXECUTABLE(mean_curvature_normals_test mean_curvature_normals_test.cpp)
ADD_EXECUTABLE(vertex_normals_test vertex_normals_test.cpp)
ADD_EXECUTABLE(type_size_test type_size_test.cpp)
ADD_EXECUTABLE(enumerate_test enumerate_test.cpp)
ADD_EXECUTABLE(range_test range_test.cpp)
ADD_EXECUTABLE(zip_test zip_test.cpp)
ADD_EXECUTABLE(reverse_test reverse_test.cpp)
ADD_EXECUTABLE(cycle_test cycle_test.cpp)
ADD_EXECUTABLE(iterator_interval_test iterator_interval_test.cpp)
ADD_EXECUTABLE(element_filter_test element_filter_test.cpp)
ADD_EXECUTABLE(slice_filter_test slice_filter_test.cpp)
ADD_EXECUTABLE(barycentric_test barycentric_test.cpp)
ADD_EXECUTABLE(kdtree_test kdtree_test.cpp)
ADD_EXECUTABLE(vertex_pruning_test vertex_pruning_test.cpp)
ADD_EXECUTABLE(opposite_volumes_test opposite_volumes_test.cpp)
ADD_EXECUTABLE(cot_laplacian_test cot_laplacian_test.cpp)

ADD_EXECUTABLE(colvector_loop_test colvector_loop_test.cpp)

ADD_EXECUTABLE(stl2eigen_test stl2eigen_test.cpp)
TARGET_LINK_LIBRARIES(stl2eigen_test mtao_core)

ADD_EXECUTABLE(bounding_box_test bounding_box_test.cpp)
TARGET_LINK_LIBRARIES(bounding_box_test mtao_core)

ADD_EXECUTABLE(horner_evaluation_test horner_evaluation_test.cpp)
ADD_EXECUTABLE(log_test log_test.cpp)
TARGET_LINK_LIBRARIES(log_test mtao_core)
ADD_EXECUTABLE(profiler_test profiler_test.cpp)
TARGET_LINK_LIBRARIES(profiler_test mtao_core)

ADD_EXECUTABLE(tuple_call_test tuple_call_test.cpp)

ADD_EXECUTABLE(halfedge_test halfedge_test.cpp)
TARGET_LINK_LIBRARIES(halfedge_test mtao_geometry)
ADD_EXECUTABLE(strand_test strand_test.cpp)
TARGET_LINK_LIBRARIES(strand_test mtao_geometry)
ADD_EXECUTABLE(halfedge_topology_test halfedge_topology_test.cpp)
TARGET_LINK_LIBRARIES(halfedge_topology_test mtao_geometry)
ADD_EXECUTABLE(halfedge_submesh_test halfedge_submesh_test.cpp)
TARGET_LINK_LIBRARIES(halfedge_submesh_test mtao_geometry)
ADD_EXECUTABLE(embedded_halfedge_test embedded_halfedge_test.cpp)
TARGET_LINK_LIBRARIES(embedded_halfedge_test mtao_geometry)

#ADD_EXECUTABLE(static_iteration_test src/static_iteration_test.cpp)
ADD_EXECUTABLE(combination_test combination_test.cpp)
ADD_EXECUTABLE(staggered_grid_test staggered_grid_test.cpp)
ADD_EXECUTABLE(staggered_grid_util_test staggered_grid_util_test.cpp)
ADD_EXECUTABLE(laplacian_smoothing_test laplacian_smoothing_test.cpp)
TARGET_LINK_LIBRARIES(laplacian_smoothing_test mtao_core)

ADD_EXECUTABLE(disjoint_set_test disjoint_set_test.cpp)
ADD_EXECUTABLE(static_loop_test static_loop_test.cpp)

ADD_EXECUTABLE(cltest cltest.cpp)
TARGET_LINK_LIBRARIES(cltest mtao_core)

ADD_EXECUTABLE(hotkey_test hotkey_test.cpp)
TARGET_LINK_LIBRARIES(hotkey_test mtao_core)

ADD_EXECUTABLE(imgui_builtin_example 
    ${IMGUI_PATH}/examples/example_glfw_opengl3/main.cpp
    ${IMGUI_PATH}/imgui_demo.cpp
    )
TARGET_INCLUDE_DIRECTORIES(imgui_builtin_example PRIVATE
    ${IMGUI_PATH}/examples)
TARGET_COMPILE_DEFINITIONS(imgui_builtin_example PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLEW)
TARGET_LINK_LIBRARIES(imgui_builtin_example  mtao_opengl GLEW)

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
