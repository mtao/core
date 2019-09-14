CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(interpolation2d
    main.cpp
    curve.cpp
    )

ADD_EXECUTABLE(interp_coeffs
    coeffs_test.cpp
    curve.cpp
    )
TARGET_LINK_LIBRARIES(interpolation2d mtao::visualization mtao::geometry)
TARGET_LINK_LIBRARIES(interp_coeffs mtao::geometry)
