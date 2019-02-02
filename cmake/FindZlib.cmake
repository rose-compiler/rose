# Zlib detection for ROSE.
#
#  INPUTS:
#    ZLIB_ROOT       -- Zlib installation directory or "no" or empty
#                       * If "no" then do not use Zlib and do not search for it.
#                       * If empty use Zlib if found, no error if not found
#                       * Else require Zlib to exist at specified location and use it
#
#  OUTPUTS:
#    ZLIB_FOOUND     -- Boolean: whether the Zlib library was found.
#    ZLIB_LIBRARY    -- String: full name of Zlib library of a string that ends with NOTFOUND
#    ZLIB_LIBRARIES  -- String: names of libraries necessary to use Zlib

macro(find_zlib)
  if("${ZLIB_ROOT}" STREQUAL "no")
    message(FATAL_ERROR "Zlib is required by Boost and cannot be disabled")

  else()
    # Header files.
    if("${ZLIB_ROOT}" STREQUAL "")
      # no extra include directories necessary
    else()
      include_directories("${ZLIB_ROOT}/include")
    endif()

    # Zlib library.
    if("${ZLIB_ROOT}" STREQUAL "")
      find_library(ZLIB_LIBRARY NAMES z)
    else()
      find_library(ZLIB_LIBRARY NAMES z PATHS "${ZLIB_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
    if(ZLIB_LIBRARY)
      set(ZLIB_FOUND TRUE)
      set(ZLIB_LIBRARIES z)
    endif()

    # Error if not found?
    if((NOT ("${ZLIB_ROOT}" STREQUAL "")) AND NOT ZLIB_FOUND)
      message(FATAL_ERROR "Zlib requested by user at '${ZLIB_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "ZLIB_ROOT       = '${ZLIB_ROOT}'")
    message(STATUS "ZLIB_FOUND      = '${ZLIB_FOUND}'")
    if(ZLIB_FOUND)
      message(STATUS "ZLIB_LIBRARY    = '${ZLIB_LIBRARY}'")
      message(STATUS "ZLIB_LIBRARIES  = '${ZLIB_LIBRARIES}'")
    endif()
  endif()
endmacro()
