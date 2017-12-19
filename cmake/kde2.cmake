# Standard KDE2 directories
set(KDE2_APPSDIR ${CMAKE_INSTALL_FULL_DATADIR}/apps CACHE STRING "KDE2 Apps dir")
set(KDE2_CONFDIR ${CMAKE_INSTALL_FULL_DATADIR}/config CACHE STRING "KDE2 CONFIG dir")
set(KDE2_DATADIR ${CMAKE_INSTALL_FULL_DATADIR} CACHE STRING "KDE2 Data dir")
set(KDE2_BINDIR ${CMAKE_INSTALL_FULL_BINDIR} CACHE STRING "KDE2 Bin dir")
set(KDE2_ICONDIR ${CMAKE_INSTALL_FULL_DATADIR}/icons CACHE STRING "KDE2 Icons dir")
set(KDE2_LOCALE ${CMAKE_INSTALL_FULL_LOCALEDIR} CACHE STRING "KDE2 Locale dir")
set(KDE2_MIMEDIR ${CMAKE_INSTALL_FULL_DATADIR}/mimelnk CACHE STRING "KDE2 Mime dir")
set(KDE2_SERVICESDIR ${CMAKE_INSTALL_FULL_DATADIR}/services CACHE STRING "KDE2 Services dir")
set(KDE2_SERVICETYPESDIR ${CMAKE_INSTALL_FULL_DATADIR}/servicetypes CACHE STRING "KDE2 ServiceTypes dir")
set(KDE2_SOUNDDIR ${CMAKE_INSTALL_FULL_DATADIR}/sound CACHE STRING "KDE2 Sound dir")
set(KDE2_TEMPLATESDIR ${CMAKE_INSTALL_FULL_DATADIR}/templates CACHE STRING "KDE2 Templates dir")
set(KDE2_WALLPAPERDIR ${CMAKE_INSTALL_FULL_DATADIR}/wallpapers CACHE STRING "KDE2 wallpapers dir")
set(KDE2_HTMLDIR ${CMAKE_INSTALL_FULL_DOCDIR}/HTML CACHE STRING "KDE2 Doc dir")
set(KDE2_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR}/kde2 CACHE STRING "KDE2 include dir")

include("${CMAKE_CURRENT_LIST_DIR}/kde2_library.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_kidl.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_module.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_icon.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_utils.cmake")

# The unfortunate global definitions
add_definitions(-DQT_NO_TRANSLATION -DQT_CLEAN_NAMESPACE -DQT_NO_COMPAT -DQT_NO_ASCII_CAST)


