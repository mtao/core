CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(interpolation2d
    main.cpp
    curve.cpp
    )

ADD_EXECUTABLE(interp_coeffs
    coeffs_test.cpp
    curve.cpp
    )
FIND_PACKAGE(OpenMP)
IF(OpenMP_FOUND)
    SET(OMP_TARGET OpenMP::OpenMP_CXX)
ENDIF(OpenMP_FOUND)

TARGET_LINK_LIBRARIES(interpolation2d mtao::visualization mtao::geometry
    ${OMP_TARGET})
TARGET_LINK_LIBRARIES(interp_coeffs mtao::geometry)
