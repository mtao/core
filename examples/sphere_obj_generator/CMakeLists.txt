CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
ADD_EXECUTABLE(sphere_obj_generator
    main.cpp
    )

TARGET_LINK_LIBRARIES(sphere_obj_generator mtao::common mtao::geometry)
