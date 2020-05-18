
## Sources Setup

configure_file(../include/msqlitecpp/version.h.in
        ../include/msqlitecpp/version.h
        @ONLY
        )

set(SOURCE_FILES
        v2/storage.cpp
        v2/statement.cpp
        utils/make_unique.h
        )

set(PUB_HEADERS
        ../include/msqlitecpp/v2/msqlitecpp.h
        ../include/msqlitecpp/v2/exceptions.h
        ../include/msqlitecpp/v2/storage.h
        ../include/msqlitecpp/v2/statement.h
        ../include/msqlitecpp/v2/fields.h
        ../include/msqlitecpp/v2/createstatement.h
        ${CMAKE_BINARY_DIR}/include/msqlitecpp/version.h
        )


if (NOT DISABLE_V1)
    set(SOURCE_FILES ${SOURCE_FILES}
            v1/sqlitestorage.cpp
            v1/sqlitetable.cpp
            v1/sqlitetransaction.cpp
            v1/sqlitestatement.cpp
            version.cpp
            )

    set(PUB_HEADERS ${PUB_HEADERS}
            ../include/msqlitecpp/v1/clauses.h
            ../include/msqlitecpp/v1/createstatement.h
            ../include/msqlitecpp/v1/deletestatement.h
            ../include/msqlitecpp/v1/insertstatement.h
            ../include/msqlitecpp/v1/msqlitecpp.h
            ../include/msqlitecpp/v1/operators.h
            ../include/msqlitecpp/v1/selectstatement.h
            ../include/msqlitecpp/v1/sqlformatters_helpers.h
            ../include/msqlitecpp/v1/sqlitefielddef.h
            ../include/msqlitecpp/v1/sqlitefieldsop.h
            ../include/msqlitecpp/v1/sqlitestatement.h
            ../include/msqlitecpp/v1/sqlitestatementformatters.h
            ../include/msqlitecpp/v1/sqlitestorage.h
            ../include/msqlitecpp/v1/sqlitetable.h
            ../include/msqlitecpp/v1/sqlitetransaction.h
            ../include/msqlitecpp/v1/updatestatement.h
            ../include/msqlitecpp/v1/sqlconstants.h
            )
endif ()
## Target setup

add_library(msqlitecpp ${SOURCE_FILES} ${PUB_HEADERS})

set_target_properties(msqlitecpp PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        EXPORT_NAME msqlitecpp
        PUBLIC_HEADER "${PUB_HEADERS}"
        )

target_include_directories(msqlitecpp
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
        ${SQLite3_INCLUDE_DIRS}
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/include
        )

if (NOT WIN32)
    target_compile_options(msqlitecpp PRIVATE -Werror)
endif ()

# Workaround the C1001 bug with microsoft compilers
if (WIN32)
    target_compile_options(msqlitecpp PUBLIC /permissive-)
endif ()

target_compile_features(msqlitecpp PUBLIC cxx_std_14)

target_compile_definitions(msqlitecpp PRIVATE BUILD_MSQLITECPP)

target_link_libraries(msqlitecpp
        PRIVATE
        ${SQLite3_LIBRARIES})

install(TARGETS msqlitecpp
        EXPORT msqlitecpp-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

install(FILES "${CMAKE_BINARY_DIR}/include/msqlitecpp/version.h" # source directory
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/msqlitecpp/" # target directory
        )

install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/msqlitecpp/v2" # source directory
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/msqlitecpp/" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h" # select header files
        )

install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/msqlitecpp/utils" # source directory
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/msqlitecpp/" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h" # select header files
        )

if (NOT DISABLE_V1)
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/msqlitecpp/v1" # source directory
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/msqlitecpp/" # target directory
            FILES_MATCHING # install only matched files
            PATTERN "*.h" # select header files
            )
endif ()

install(EXPORT msqlitecpp-export
        FILE
        msqlitecppTargets.cmake
        NAMESPACE
        msqlitecpp::
        DESTINATION
        ${INSTALL_EXPORTS_DIR}
        )

set(INCLUDE_INSTALL_DIR include/)
set(LIB_INSTALL_DIR lib/)
set(SYSCONFIG_INSTALL_DIR etc/msqlitecpp/)

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)

configure_package_config_file(
        ${CMAKE_SOURCE_DIR}/cmake/msqlitecppConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/msqlitecppConfig.cmake
        INSTALL_DESTINATION ${INSTALL_EXPORTS_DIR}
        PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR)

write_basic_package_version_file(
        ${CMAKE_BINARY_DIR}/msqlitecppConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/msqlitecppConfig.cmake
        ${CMAKE_BINARY_DIR}/msqlitecppConfigVersion.cmake
        DESTINATION ${INSTALL_EXPORTS_DIR}
        )
