
add_custom_command(
    OUTPUT kmedia2.h kmedia2.cc
    COMMAND mcopidl -I${CMAKE_CURRENT_SOURCE_DIR}/../flow/ -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kmedia2.idl
    )
add_custom_command(
    OUTPUT soundserver.h soundserver.cc
    COMMAND mcopidl -I${CMAKE_CURRENT_SOURCE_DIR}/../flow/ -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/soundserver.idl
    )

kde2_library(libsoundserver
    SOURCES
        soundserver.cc
        kmedia2.cc
    LIBS
        kde2::mcop
        kde2::artsflow
    VERSION
        3.0.0
    )
set(CMAKE_INCLUDE_CURRENT_DIR ON)
target_include_directories(libsoundserver PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
