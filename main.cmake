
include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${SQLITE_INCLUDE_DIRS}
)

set(SOURCE_FILES
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
        src/sqlitestorage.cpp
        src/sqlitetable.cpp
        src/sqlitetransaction.cpp
        src/sqlitestatement.cpp
        src/utils/make_unique.h
        )


if (ENABLE_SQLITE_AMALGAMATION)
    list(APPEND SOURCE_FILES ${SQLITE_SOURCE_PATH}/sqlite3.c)
    include_directories(${SQLITE_INCLUDE_PATH})
endif (ENABLE_SQLITE_AMALGAMATION)

add_library(msqlitecpp ${LINK_SHARED} ${SOURCE_FILES})

target_link_libraries(msqlitecpp ${SQLITE_LIB})

install(TARGETS msqlitecpp DESTINATION usr/lib COMPONENT runtime)

