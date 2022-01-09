INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)




FUNCTION(ADD_CATCHTEST BINNAME)
    ADD_EXECUTABLE(${BINNAME}_test main.cpp ${ARGN})
    TARGET_LINK_LIBRARIES(${BINNAME}_test PRIVATE mtao::common Catch2::Catch2 fmt)
    add_test(NAME ${BINNAME} COMMAND ${BINNAME}_test)
    set_property(TARGET ${BINNAME}_test PROPERTY CXX_STANDARD 20)
ENDFUNCTION()


ADD_CATCHTEST(core
    tuple_index_util_test.cpp
    )

ADD_CATCHTEST(geometry
    angle_test.cpp 
    cell_list_test.cpp
    centroid_test.cpp
    cyclic_order_test.cpp
    dual_edge_lengths_test.cpp
    boundary_elements_test.cpp
    dual_volume_test.cpp
    edges_to_plcurves_test.cpp
    grid_data_test.cpp
    grid_test.cpp
    halfedge_topology_test.cpp
    multi_loop_test.cpp
    read_obj_test.cpp
    stack_meshes_test.cpp
    staggered_grid_boundary_test.cpp
    staggered_grid_test.cpp
    triangle_monomial_integrals_test.cpp
    tetrahedron_monomial_integrals_test.cpp
    edge_monomial_integrals_test.cpp
    unique_simplices_test.cpp
    winding_number_test.cpp
    edges_to_polygons_test.cpp
    earclipping_test.cpp
    triangulation_test.cpp
    )
target_link_libraries(geometry_test PRIVATE mtao::geometry)


ADD_CATCHTEST(eigen
    sparse_blocks_test.cpp
    sparse_diagonal_repmats_test.cpp
    diagonal_to_sparse_test.cpp
    shape_check_tests.cpp
    solid_angle_test.cpp
    )

ADD_CATCHTEST(linalg
    conjugate_gradient_tests.cpp
    gram_schmidt_test.cpp
    krylov_tests.cpp
    solver_tests.cpp
    matrix_islands_test.cpp
    )

ADD_CATCHTEST(algebra
    composite_test.cpp
    pascal_triangle_test.cpp
    sort_with_permutation_count_test.cpp
    partial_order_to_dag_test.cpp
    )

ADD_CATCHTEST(simulation
    laplacian_test.cpp
    )

ADD_CATCHTEST(quadrature
    quadrature_test.cpp
    )

ADD_CATCHTEST(reindex
    reindexer_test.cpp
    )
if(MTAO_USE_TBB)
    #ADD_CATCHTEST(tbb
    #    tbb_parloop_test.cpp 
    #    )
endif()


add_subdirectory(run_tests)
