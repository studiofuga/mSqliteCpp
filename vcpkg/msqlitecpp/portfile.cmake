# Common Ambient Variables:
#   CURRENT_BUILDTREES_DIR    = ${VCPKG_ROOT_DIR}\buildtrees\${PORT}
#   CURRENT_PACKAGES_DIR      = ${VCPKG_ROOT_DIR}\packages\${PORT}_${TARGET_TRIPLET}
#   CURRENT_PORT_DIR          = ${VCPKG_ROOT_DIR}\ports\${PORT}
#   PORT                      = current port name (zlib, etc)
#   TARGET_TRIPLET            = current triplet (x86-windows, x64-windows-static, etc)
#   VCPKG_CRT_LINKAGE         = C runtime linkage type (static, dynamic)
#   VCPKG_LIBRARY_LINKAGE     = target library linkage type (static, dynamic)
#   VCPKG_ROOT_DIR            = <C:\path\to\current\vcpkg>
#   VCPKG_TARGET_ARCHITECTURE = target architecture (x64, x86, arm)
#

include(vcpkg_common_functions)
set(SOURCE_PATH ${CURRENT_BUILDTREES_DIR}/src/mSqliteCpp-1.99.6.3)
vcpkg_download_distfile(ARCHIVE
		URLS "https://github.com/studiofuga/mSqliteCpp/archive/v1.99.6.3.tar.gz"
		FILENAME "mSqliteCpp-1.99.6.3.tar.gz"
		SHA512 0
		)
vcpkg_extract_source_archive(${ARCHIVE})

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA # Disable this option if project cannot be built with Ninja
    OPTIONS -DENABLE_TEST=OFF -DENABLE_PROFILER=OFF -DENABLE_SQLITE_AMALGAMATION=OFF -DENABLE_CODE_COVERAGE=OFF
)

vcpkg_install_cmake()

# Remove the EXTRA include files

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)

# move the LIB files to the correct location

file(COPY
        ${CURRENT_PACKAGES_DIR}/debug/share/msqlitecpp/msqlitecppTargets-debug.cmake
        DESTINATION
        ${CURRENT_PACKAGES_DIR}/share/msqlitecpp)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/share)

# Handle copyright

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/msqlitecpp RENAME copyright)

# Post-build test for cmake libraries
# vcpkg_test_cmake(PACKAGE_NAME msqlitecpp)
