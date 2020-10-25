
add_custom_command(
    OUTPUT artsflow.h artsflow.cc
    COMMAND mcopidl ${CMAKE_CURRENT_SOURCE_DIR}/artsflow.idl
    )

kde2_library(libartsflow
    SOURCES
        artsflow.cc
    LIBS
        kde2::mcop
    VERSION
        3.0.0
    )

