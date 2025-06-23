project(bbcppexamples)

# C++ example
add_executable(bbcpp
    main.cpp
    cxxopts.hpp)

target_link_libraries(bbcpp
    bbcppstatic
)

# C example
add_executable(bbcpp_c_example
    example_c.c)

target_link_libraries(bbcpp_c_example
    bbcppc
)

# Set C standard for the C example
set_target_properties(bbcpp_c_example PROPERTIES
    C_STANDARD 99
)

# Simple C example
add_executable(bbcpp_simple_example
    simple_c_example.c)

target_link_libraries(bbcpp_simple_example
    bbcppc
)

# Set C standard for the simple C example
set_target_properties(bbcpp_simple_example PROPERTIES
    C_STANDARD 99
)
