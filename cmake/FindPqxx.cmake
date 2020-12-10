# Pqxx detection for ROSE.
#
#  INPUTS:
#    PQXX_ROOT       -- PQXX installation directory or "no" or empty
#                       * If "no" then do not use PQXX and do not search for it.
#                       * If empty use PQXX if found, no error if not found
#                       * Else require PQXX to exist at specified location and use it
#
#  OUTPUTS:
#    PQXX_FOOUND     -- Boolean: whether the PQXX library was found.
#    PQXX_LIBRARY    -- String: full name of PQXX library of a string that ends with NOTFOUND
#    PQXX_LIBRARIES  -- String: names of libraries necessary to use PQXX

macro(find_pqxx)
  if("${PQXX_ROOT}" STREQUAL "no")
    # Do not use PQXX, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(PQXX_FOUND FALSE)
    set(PQXX_LIBRARY "")
    set(PQXX_LIBRARIES "")

  else()
    # Header files.
    if("${PQXX_ROOT}" STREQUAL "")
      # no extra include directories necessary
    else()
      include_directories("${PQXX_ROOT}/include")
    endif()

    # PQXX library.
    if("${PQXX_ROOT}" STREQUAL "")
      find_library(PQXX_LIBRARY NAMES pqxx)
    else()
      find_library(PQXX_LIBRARY NAMES pqxx PATHS "${PQXX_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
    if(PQXX_LIBRARY)
      set(PQXX_FOUND TRUE)
      set(PQXX_LIBRARIES pqxx)
    endif()

    # Error if not found?
    if((NOT ("${PQXX_ROOT}" STREQUAL "")) AND NOT PQXX_FOUND)
      message(FATAL_ERROR "Pqxx requested by user at '${PQXX_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "PQXX_ROOT       = '${PQXX_ROOT}'")
    message(STATUS "PQXX_FOUND      = '${PQXX_FOUND}'")
    if(PQXX_FOUND)
      message(STATUS "PQXX_LIBRARY    = '${PQXX_LIBRARY}'")
      message(STATUS "PQXX_LIBRARIES  = '${PQXX_LIBRARIES}'")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_LIBPQXX ${PQXX_FOUND})
endmacro()
