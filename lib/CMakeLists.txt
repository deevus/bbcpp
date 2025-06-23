project(bbcpplib)

set(SOURCE_FILES
    bbcpputils.cpp
    BBDocument.cpp
    bbcpp_c.cpp
    bbcpp_simple.c
)

set(HEADER_FILES
    bbcpputils.h
    BBDocument.h
    bbcpp_c.h
    bbcpp_simple.h
)

add_library(bbcppstatic STATIC
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

install(FILES ${HEADER_FILES} DESTINATION include)

# Add C wrapper library
add_library(bbcppc STATIC
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

# Set C compatibility for the C wrapper
set_target_properties(bbcppc PROPERTIES
    C_STANDARD 99
    CXX_STANDARD 17
)

if (BUILD_SHARED_LIBS)
    add_library(bbcppshared SHARED
        ${SOURCE_FILES}
        ${HEADER_FILES})

    set_target_properties(bbcppshared PROPERTIES OUTPUT_NAME "bbcpp")
    set_target_properties(bbcppshared PROPERTIES VERSION 0.1 SOVERSION 1)
    install(TARGETS bbcppshared
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR})

    # Add shared C wrapper library
    add_library(bbcppcshared SHARED
        ${SOURCE_FILES}
        ${HEADER_FILES})

    set_target_properties(bbcppcshared PROPERTIES OUTPUT_NAME "bbcppc")
    set_target_properties(bbcppcshared PROPERTIES VERSION 0.1 SOVERSION 1)
    set_target_properties(bbcppcshared PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 17
    )
    install(TARGETS bbcppcshared
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR})
endif()
