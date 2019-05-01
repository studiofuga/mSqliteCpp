
## Sources Setup

set(SOURCE_FILES
        src/sqlitestorage.cpp
        src/sqlitetable.cpp
        src/sqlitetransaction.cpp
        src/sqlitestatement.cpp
        src/utils/make_unique.h
        )

set(PUB_HEADERS
        include/clauses.h
        include/createstatement.h
        include/deletestatement.h
        include/insertstatement.h
        include/msqlitecpp.h
        include/operators.h
        include/selectstatement.h
        include/sqlformatters_helpers.h
        include/sqlitefielddef.h
        include/sqlitefieldsop.h
        include/sqlitestatement.h
        include/sqlitestatementformatters.h
        include/sqlitestorage.h
        include/sqlitetable.h
        include/sqlitetransaction.h
        include/updatestatement.h
        include/sqlconstants.h)

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
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
            ${Boost_INCLUDE_DIRS}
            ${SQLITE_INCLUDE_DIRS}
            ${CMAKE_SOURCE_DIR}/src
        )

if (NOT WIN32)
    target_compile_options(msqlitecpp PRIVATE -Werror)
endif()

# Workaround the C1001 bug with microsoft compilers
if (WIN32)
    target_compile_options(msqlitecpp PUBLIC /permissive-)
endif()

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
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )

install(EXPORT msqlitecpp-export
        FILE
            msqlitecppTargets.cmake
        NAMESPACE
            msqlitecpp::
        DESTINATION
            ${INSTALL_EXPORTS_DIR}
        )

install(FILES
        ${CMAKE_SOURCE_DIR}/cmake/msqlitecppConfig.cmake
        DESTINATION ${INSTALL_EXPORTS_DIR}
        )
