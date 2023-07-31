# Capstone detection for ROSE.
#
#  INPUTS:
#    CAPSTONE_ROOT       -- Capstone installation directory or "no" or empty
#                       * If "no" then do not use Capstone and do not search for it.
#                       * If empty use Capstone if found, no error if not found
#                       * Else require Capstone to exist at specified location and use it
#
#  OUTPUTS:
#    CAPSTONE_FOUND      -- Boolean: whether the Capstone library was found.
#    CAPSTONE_LIBRARY    -- String: full name of Capstone library of a string that ends with NOTFOUND
#    CAPSTONE_LIBRARIES  -- String: names of libraries necessary to use Capstone

macro(find_capstone)
  if("${CAPSTONE_ROOT}" STREQUAL "no")
    # Do not use Capstone, and therefore do not even search for it. Make sure all outputs are cleared
    # to avoid problems with users maybe setting them.
    set(CAPSTONE_FOUND FALSE)
    set(CAPSTONE_LIBRARY "")
    set(CAPSTONE_LIBRARIES "")
  else()

    # Header files.
    if("${CAPSTONE_ROOT}" STREQUAL "")
      # no extra include directories necessary
    else()
      include_directories("${CAPSTONE_ROOT}/include")
    endif()

    # Capstone library.
    if("${CAPSTONE_ROOT}" STREQUAL "")
      find_library(CAPSTONE_LIBRARY NAMES capstone)
    else()
      find_library(CAPSTONE_LIBRARY NAMES capstone PATHS "${CAPSTONE_ROOT}/lib" NO_DEFAULT_PATH)
    endif()

    if(CAPSTONE_LIBRARY)
      set(CAPSTONE_FOUND TRUE)
      set(CAPSTONE_LIBRARIES capstone)
    endif()

    # Error if not found?
    if((NOT ("${CAPSTONE_ROOT}" STREQUAL "")) AND NOT CAPSTONE_FOUND)
      message(FATAL_ERROR "Capstone requested by user at '${CAPSTONE_ROOT}' but not found")
    endif()
  endif()

  # Summarize
  if(VERBOSE)
    message(STATUS "CAPSTONE_ROOT       = '${CAPSTONE_ROOT}'")
    message(STATUS "CAPSTONE_FOUND      = '${CAPSTONE_FOUND}'")
    if(CAPSTONE_FOUND)
      message(STATUS "CAPSTONE_LIBRARY    = '${CAPSTONE_LIBRARY}'")
      message(STATUS "CAPSTONE_LIBRARIES  = '${CAPSTONE_LIBRARIES}'")
    endif()
  else()
    if(CAPSTONE_FOUND)
      message(STATUS "Found Capstone: ${CAPSTONE_LIBRARY}")
    else()
      message(STATUS "Capstone NOT found.")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_CAPSTONE ${CAPSTONE_FOUND})
endmacro()
