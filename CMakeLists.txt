cmake_minimum_required(VERSION 2.8.9)
PROJECT(MtaoCore)

SET(CMAKE_RUNTIME_OUTPUT_DIR ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIR ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIR ${PROJECT_BINARY_DIR}/lib)

ADD_DEFINITIONS(-std=c++14)
IF(CMAKE_BUILD_TYPE MATCHES Debug)
ADD_DEFINITIONS(-ggdb)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
ADD_DEFINITIONS(-Wall)
ADD_DEFINITIONS(-Wno-deprecated-declarations)
ADD_DEFINITIONS(-Wno-unused-variable)



# Include dirs
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include
    /usr/local/include/eigen3
)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
#SET(CMAKE_AUTOMOC ON)
#find_package(Qt5Widgets)


#FIND_PACKAGE(Protobuf)
IF ( PROTOBUF_FOUND )
    INCLUDE_DIRECTORIES( ${PROTOBUF_INCLUDE_DIR} )
    SET(PROTOBUF_IMPORT_DIRS proto)
    PROTOBUF_GENERATE_CPP(GRID_PROTO_SRCS GRID_PROTO_HDRS proto/grid.proto)
    SET(PROTOBUF_LIBS ${PROTOBUF_LIBRARY})
ENDIF(PROTOBUF_FOUND)




#ADD_EXECUTABLE(advection_test src/advection_test.cpp include/grid.h include/grid_utils.h
#    src/scalar_function_examples.cpp
#    include/discrete_scalar_function.h include/scalar_function.h
#    include/scalar_function_examples.h
#    include/arguments.h)
#
#TARGET_LINK_LIBRARIES(advection_test pthread)

SET(CORE_HEADERS
    include/compat.h
    include/coord.h
    include/conversion.h
    include/static_iteration.h
    include/types.h
    include/util.h
    )

SET(GRID_HEADERS
    ${CORE_HEADERS}
    include/grid/grid.h
    include/grid/grid_storage.h
    include/grid/grid_utils.h
    include/grid/index_map.h
    include/grid/index_map_iterator.h
    )

SET(LEVELSET_HEADERS
    ${GRID_HEADERS}
    include/levelset/levelset.h
    include/levelset/ops.h
    include/levelset/levelset_example.h
    include/levelset/primitives.h
    include/levelset/transformations.h
    include/levelset/transformer.h
    include/levelset/discrete.h
    include/levelset/csg.h
    include/levelset/animation.h
    include/levelset/particle_sampler.h
    )


ADD_EXECUTABLE(grid_test src/grid_test.cpp ${GRID_HEADERS}
    )
ADD_EXECUTABLE(levelset_test src/levelset_test.cpp ${LEVELSET_HEADERS}
    )
ADD_EXECUTABLE(multi_loop_test src/multi_loop_test.cpp include/grid/grid.h include/grid/grid_utils.h
    )
ADD_EXECUTABLE(static_type_iteration_test src/static_type_iteration_test.cpp
    include/static_iteration.h include/util.h)

ADD_EXECUTABLE(static_iteration_test src/static_iteration_test.cpp
    include/static_iteration.h include/util.h)

#ADD_EXECUTABLE(particle_sampling_test src/particle_sampling_test.cpp include/grid.h include/grid_utils.h
#    include/arguments.h include/particle_sampler.h)
#
#
#ADD_EXECUTABLE(parsing_test src/parsing_test.cpp include/grid.h include/grid_utils.h
#    include/arguments.h src/parsing_test.cpp src/data_parser.cpp
#    src/coloring.cpp
#    src/scalar_function_examples.cpp
#    include/data_parser.h)
#
#ADD_EXECUTABLE(func_print_test src/function_print_test.cpp include/grid.h include/grid_utils.h
#    src/scalar_function_examples.cpp
#    src/coloring.cpp
#    include/coloring.h
#    include/discrete_scalar_function.h include/scalar_function.h
#    include/scalar_function_examples.h
#    include/arguments.h)
#
#
#
