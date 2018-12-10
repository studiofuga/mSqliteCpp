

get_filename_component(msqlitecpp_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

list(APPEND CMAKE_MODULE_PATH ${msqlitecpp_CMAKE_DIR})

find_package(Boost 1.55 REQUIRED)
#list(REMOVE_AT CMAKE_MODULE_PATH -1)

if(NOT TARGET msqlitecpp::msqlitecpp)
    include("${msqlitecpp_CMAKE_DIR}/msqlitecppTargets.cmake")
endif()

set(msqlitecpp_LIBRARIES msqlitecpp::msqlitecpp)