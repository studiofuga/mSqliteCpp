set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "cmake-build*;\\\\.git;\\\\.idea")
include(CPack)

if (UNIX)
    include(GNUInstallDirs)
    set(INSTALL_EXPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/msqlitecpp)
    unset(CMAKE_INSTALL_INCLUDEDIR)
else ()
    set(CMAKE_INSTALL_LIBDIR lib)
    set(CMAKE_INSTALL_BINDIR bin)
    set(CMAKE_INSTALL_INCLUDEDIR include)
endif ()


if (WIN32)
    set (INSTALL_EXPORTS_DIR lib/cmake)
endif(WIN32)