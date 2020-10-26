set(knotify_SRCS knotify.cpp)

qt2_wrap_cpp(knotify_SRCS
    SOURCES
        knotify.h
    )

add_executable(knotify ${knotify_SRCS})
target_link_libraries(knotify
    Qt::Qt2
    kde2::kdecore
    kde2::kdeui
    kde2::soundserver
    kde2::qtmcop
    )
