ADD_DEFINITIONS(-g)
ADD_EXECUTABLE(grid_test grid_test.cpp)
ADD_EXECUTABLE(levelset_test levelset_test.cpp)
ADD_EXECUTABLE(multi_loop_test multi_loop_test.cpp)
ADD_EXECUTABLE(static_type_iteration_test static_type_iteration_test.cpp)
ADD_EXECUTABLE(eigen_interweave_test eigen_interweave_test.cpp)
ADD_EXECUTABLE(eigen_stack_test eigen_stack_test.cpp)
ADD_EXECUTABLE(volume_test volume_test.cpp)
ADD_EXECUTABLE(boundary_test boundary_test.cpp)
ADD_EXECUTABLE(sphere_test sphere_test.cpp)
ADD_EXECUTABLE(area_normals_test area_normals_test.cpp)

ADD_EXECUTABLE(colvector_loop_test colvector_loop_test.cpp)

ADD_EXECUTABLE(bounding_box_test bounding_box_test.cpp)
TARGET_LINK_LIBRARIES(bounding_box_test mtao_core)

ADD_EXECUTABLE(log_test log_test.cpp)
TARGET_LINK_LIBRARIES(log_test mtao_core)


ADD_EXECUTABLE(halfedge_test halfedge_test.cpp)
TARGET_LINK_LIBRARIES(halfedge_test mtao_geometry)

#ADD_EXECUTABLE(static_iteration_test src/static_iteration_test.cpp)

ADD_EXECUTABLE(cltest cltest.cpp)
TARGET_LINK_LIBRARIES(cltest mtao_core)

ADD_EXECUTABLE(hotkey_test hotkey_test.cpp)
TARGET_LINK_LIBRARIES(hotkey_test mtao_core)

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
