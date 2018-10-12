

get_filename_component(MSqliteCpp_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

list(APPEND CMAKE_MODULE_PATH ${MSqliteCpp_CMAKE_DIR})

find_package(Boost 1.55 REQUIRED)
list(REMOVE_AT CMAKE_MODULE_PATH -1)

if(NOT TARGET MSqliteCpp::MSqliteCpp)
    include("${JSONUtils_CMAKE_DIR}/MSqliteCppTargets.cmake")
endif()

set(MSqliteCpp_LIBRARIES MSqliteCpp::MSqliteCpp)