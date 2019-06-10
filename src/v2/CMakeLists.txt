
## Sources Setup

set(SOURCE_FILES
        storage.cpp
        statement.cpp
        ../utils/make_unique.h
        )

set(PUB_HEADERS
        ../../include/msqlitecpp/v2/msqlitecpp.h
        ../../include/msqlitecpp/v2/exceptions.h
        ../../include/msqlitecpp/v2/storage.h
        ../../include/msqlitecpp/v2/statement.h
        ../../include/msqlitecpp/v2/fields.h)

## Target setup

add_library(msqlitecpp2 ${SOURCE_FILES} ${PUB_HEADERS})

set_target_properties(msqlitecpp2 PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        EXPORT_NAME msqlitecpp
        PUBLIC_HEADER "${PUB_HEADERS}"
        )

target_include_directories(msqlitecpp2
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${SQLITE_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src
        )

if (NOT WIN32)
    target_compile_options(msqlitecpp2 PRIVATE -Werror)
endif ()

# Workaround the C1001 bug with microsoft compilers
if (WIN32)
    target_compile_options(msqlitecpp2 PUBLIC /permissive-)
endif ()

target_compile_features(msqlitecpp2 PUBLIC cxx_std_14)

target_compile_definitions(msqlitecpp2 PRIVATE BUILD_MSQLITECPP)

target_link_libraries(msqlitecpp2
        PRIVATE
        ${SQLite3_LIBRARIES})

install(TARGETS msqlitecpp2
        EXPORT msqlitecpp2-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/msqlitecpp/v2
        )

install(EXPORT msqlitecpp2-export
        FILE
        msqlitecpp2Targets.cmake
        NAMESPACE
        msqlitecpp2::
        DESTINATION
        ${INSTALL_EXPORTS_DIR}
        )

install(FILES
        ${CMAKE_SOURCE_DIR}/cmake/msqlitecpp2Config.cmake
        DESTINATION ${INSTALL_EXPORTS_DIR}
        )
