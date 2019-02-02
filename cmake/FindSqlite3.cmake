# SQLite3 detection for ROSE.
#
#  INPUTS:
#    SQLITE3_ROOT   -- SQLite3 installation directory or "no" or empty
#                     * If "no" then do not use SQLite3 and do not search for it.
#                     * If empty use SQLite3 if found, no error if not found
#                     * Else require SQLite3 to exist at specified location and use it
#
#  OUTPUTS:
#    SQLITE3_FOOUND     -- Boolean: whether the sqlite3 executable or the SQLite3 library was found.
#    SQLITE3_FOUND_EXE  -- Boolean: whether the sqlite executable was found
#    SQLITE3_FOUND_LIB  -- Boolean: whether the SQLite3 library was found
#    SQLITE3_EXECUTABLE -- String:  the sqlite3 executable name
#    SQLITE3_VERSION    -- String:  SQLite3 version string, such as "4.8.1"
#    SQLITE3_LIBRARIES  -- String:  names of libraries necessary to use SQLite3

macro(find_sqlite3)
  if("${SQLITE3_ROOT}" STREQUAL "no")
    # Do not use SQLite3, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(SQLITE3_FOUND FALSE)
    set(SQLITE3_FOUND_EXE FALSE)
    set(SQLITE3_FOUND_LIB FALSE)
    set(SQLITE3_EXECUTABLE "")
    set(SQLITE3_LIBRARY "")
    set(SQLITE3_VERSION "")
    set(SQLITE3_LIBRARIES "")

  else()
    # SQLite3 executable
    if("${SQLITE3_ROOT}" STREQUAL "")
      find_program(SQLITE3_EXECUTABLE NAMES sqlite3)
    else()
      find_program(SQLITE3_EXECUTABLE NAMES sqlite3 PATHS "${SQLITE3_ROOT}/bin" NO_DEFAULT_PATH)
    endif()
    if(SQLITE3_EXECUTABLE)
      set(SQLITE3_FOUND TRUE)
      set(SQLITE3_FOUND_EXE TRUE)
      if(NOT SQLITE3_VERSION)
	execute_process(COMMAND ${SQLITE3_EXECUTABLE} --version OUTPUT_VARIABLE SQLITE3_VERSION_OUTPUT)
	string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" SQLITE3_VERSION "${SQLITE3_VERSION_OUTPUT}")
      endif()
    endif()

    # Header files.
    if(NOT("${SQLITE3_ROOT}" STREQUAL ""))
      include_directories("${SQLITE3_ROOT}/include")
    endif()
    if(SQLITE3_VERSION_H AND NOT SQLITE3_VERSION)
      message(FATAL_ERROR "SQLite3 version from sqlite3.h not implemented in cmake yet")
    endif()

    # SQLite3 library.
    if("${SQLITE3_ROOT}" STREQUAL "")
      find_library(SQLITE3_LIBRARY NAMES sqlite3)
    else()
      find_library(SQLITE3_LIBRARY NAMES sqlite3 PATHS "${SQLITE3_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
    if(SQLITE3_LIBRARY)
      set(SQLITE3_FOUND TRUE)
      set(SQLITE3_FOUND_LIB TRUE)
      set(SQLITE3_LIBRARIES sqlite3)
      if(NOT SQLITE3_VERSION)
	message(FATAL_ERROR "SQLite3 version cannot be obtained from the library")
      endif()
    endif()

    # Error if not found? [Note: extra(?) parens are due to lack of precedence documentation in cmake]
    if((NOT ("${SQLITE3_ROOT}" STREQUAL "")) AND NOT SQLITE3_FOUND)
      message(FATAL_ERROR "SQLite3 requested by user at '${SQLITE3_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "SQLITE3_ROOT       = '${SQLITE3_ROOT}'")
    message(STATUS "SQLITE3_FOUND      = '${SQLITE3_FOUND}'")
    if(SQLITE3_FOUND)
      message(STATUS "SQLITE3_VERSION    = '${SQLITE3_VERSION}'")
      message(STATUS "SQLITE3_FOUND_EXE  = '${SQLITE3_FOUND_EXE}'")
      message(STATUS "SQLITE3_FOUND_LIB  = '${SQLITE3_FOUND_LIB}'")
      if(SQLITE3_FOUND_EXE)
	message(STATUS "SQLITE3_EXECUTABLE = '${SQLITE3_EXECUTABLE}'")
      endif()
      if(SQLITE3_FOUND_LIB)
	message(STATUS "SQLITE3_LIBRARY    = '${SQLITE3_LIBRARY}'")
	message(STATUS "SQLITE3_LIBRARIES  = '${SQLITE3_LIBRARIES}'")
      endif()
    endif()
  endif()

  # ROSE variables
  set(HAVE_SQLITE3 ${SQLITE3_FOUND_LIB})
endmacro()
