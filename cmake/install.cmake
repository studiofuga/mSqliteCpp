include(CPack)

if (UNIX)
    include(GNUInstallDirs)
    set(INSTALL_EXPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/msqlitecpp)
    set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/msqlitecpp)
else ()
    set(CMAKE_INSTALL_LIBDIR lib)
    set(CMAKE_INSTALL_BINDIR bin)
    set(CMAKE_INSTALL_INCLUDEDIR include)
endif ()


if (WIN32)
    set (INSTALL_EXPORTS_DIR share/msqlitecpp)
endif(WIN32)