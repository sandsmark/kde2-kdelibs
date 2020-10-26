set(qtmcop_SRC qiomanager.cc)

qt2_wrap_cpp(qtmcop_SRC
    SOURCES
        qiomanager_p.h
    )
kde2_library(libqtmcop
    SOURCES
        ${qtmcop_SRC}
    LIBS
        Qt::Qt2
        kde2::mcop
    VERSION
        3.0.0
    )
