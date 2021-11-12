# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindSQLite3
-----------

Find the SQLite libraries, v3

IMPORTED targets
^^^^^^^^^^^^^^^^

This module defines the following :prop_tgt:`IMPORTED` target:

``SQLite::SQLite3``

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables if found:

``SQLITE3_INCLUDE_DIRS``
  where to find sqlite3.h, etc.
``SQLITE3_LIBRARY``
  the libraries to link against to use SQLite3.
``SQLITE3_VERSION``
  version of the SQLite3 library found
``SQLITE3_FOUND``
  TRUE if found

#]=======================================================================]

# Look for the necessary header
find_path(SQLite3_INCLUDE_DIR NAMES sqlite3.h)
mark_as_advanced(SQLite3_INCLUDE_DIR)

# Look for the necessary library
find_library(SQLite3_LIBRARY NAMES sqlite3 sqlite)
mark_as_advanced(SQLite3_LIBRARY)

# Extract version information from the header file
if(SQLite3_INCLUDE_DIR)
    file(STRINGS ${SQLite3_INCLUDE_DIR}/sqlite3.h _ver_line
            REGEX "^#define SQLITE_VERSION  *\"[0-9]+\\.[0-9]+\\.[0-9]+\""
            LIMIT_COUNT 1)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+"
            SQLite3_VERSION "${_ver_line}")
    unset(_ver_line)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3
        REQUIRED_VARS SQLite3_INCLUDE_DIR SQLite3_LIBRARY
        VERSION_VAR SQLite3_VERSION)

# Create the imported target
if(SQLite3_FOUND)
    set(SQLITE3_FOUND ${SQLite3_FOUND})
    set(SQLITE3_VERSION ${SQLite3_VERSION})
    set(SQLITE3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIR})
    set(SQLITE3_LIBRARY ${SQLite3_LIBRARY})
    if(NOT TARGET SQLite::SQLite3)
        add_library(SQLite::SQLite3 UNKNOWN IMPORTED)
        set_target_properties(SQLite::SQLite3 PROPERTIES
                IMPORTED_LOCATION             "${SQLITE3_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${SQLITE3_INCLUDE_DIRS}")
    endif()
endif()
