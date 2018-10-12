
## Sources Setup

set(SOURCE_FILES
        src/sqlitestorage.cpp
        src/sqlitetable.cpp
        src/sqlitetransaction.cpp
        src/sqlitestatement.cpp
        src/utils/make_unique.h
        )

set(PUB_HEADERS
        include/msqlitecpp.h
        include/sqlitefielddef.h
        include/sqlitestorage.h
        include/sqlitetable.h
        include/sqlitetransaction.h
        include/sqlitestatement.h
        include/insertstatement.h
        include/selectstatement.h
        include/clauses.h
        include/deletestatement.h
        include/updatestatement.h
        include/createstatement.h
        include/sqlformatters_helpers.h
        include/sqlitestatementformatters.h
        include/sqlitefieldsop.h
        )

if (ENABLE_SQLITE_AMALGAMATION)
    list(APPEND SOURCE_FILES ${SQLITE_SOURCE_PATH}/sqlite3.c)
    include_directories(${SQLITE_INCLUDE_PATH})
endif (ENABLE_SQLITE_AMALGAMATION)

## Target setup

add_library(msqlitecpp ${SOURCE_FILES} ${PUB_HEADERS})

set_target_properties(msqlitecpp PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        EXPORT_NAME MSqliteCpp
        PUBLIC_HEADER "${PUB_HEADERS}"
        )

target_include_directories(msqlitecpp
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
            ${SQLITE_INCLUDE_DIRS}
            ${CMAKE_SOURCE_DIR}/src
        )

target_compile_options(msqlitecpp PRIVATE -Werror)
target_compile_features(msqlitecpp PUBLIC cxx_std_14)

target_link_libraries(msqlitecpp
        PRIVATE
        ${SQLITE_LIB})

install(TARGETS msqlitecpp
        EXPORT msqlitecpp-export
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )

install(EXPORT msqlitecpp-export
        FILE
            MSqliteCppTargets.cmake
        NAMESPACE
            MSqliteCpp::
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/MSqliteCpp
        )

install(FILES
        ${CMAKE_SOURCE_DIR}/cmake/MSqliteCppConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MSqliteCpp
        )
