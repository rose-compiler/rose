# Dlib detection for ROSE.
#
#  INPUTS:
#    DLIB_ROOT       -- DLIB installation directory or "no" or empty
#                       * If "no" then do not use DLIB and do not search for it.
#                       * If empty use DLIB if found, no error if not found
#                       * Else require DLIB to exist at specified location and use it
#
#  OUTPUTS:
#    DLIB_FOOUND     -- Boolean: whether the DLIB library was found.
#    DLIB_LIBRARY    -- String: full name of DLIB library of a string that ends with NOTFOUND
#    DLIB_LIBRARIES  -- String: names of libraries necessary to use DLIB

macro(find_dlib)
  if("${DLIB_ROOT}" STREQUAL "no")
    # Do not use DLIB, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(DLIB_FOUND FALSE)
    set(DLIB_LIBRARY "")
    set(DLIB_LIBRARIES "")

  else()
    # Header files.
    if("${DLIB_ROOT}" STREQUAL "")
      # no extra include directories necessary
    else()
      include_directories("${DLIB_ROOT}/include")
    endif()

    # DLIB library.
    if("${DLIB_ROOT}" STREQUAL "")
      find_library(DLIB_LIBRARY NAMES dlib)
    else()
      find_library(DLIB_LIBRARY NAMES dlib PATHS "${DLIB_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
    if(DLIB_LIBRARY)
      set(DLIB_FOUND TRUE)
      set(DLIB_LIBRARIES dlib)
    endif()

    # Error if not found?
    if((NOT ("${DLIB_ROOT}" STREQUAL "")) AND NOT DLIB_FOUND)
      message(FATAL_ERROR "Dlib requested by user at '${DLIB_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "DLIB_ROOT       = '${DLIB_ROOT}'")
    message(STATUS "DLIB_FOUND      = '${DLIB_FOUND}'")
    if(DLIB_FOUND)
      message(STATUS "DLIB_LIBRARY    = '${DLIB_LIBRARY}'")
      message(STATUS "DLIB_LIBRARIES  = '${DLIB_LIBRARIES}'")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_DLIB ${DLIB_FOUND})
endmacro()
