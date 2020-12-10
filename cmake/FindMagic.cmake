# Magic (libmagic) detection for ROSE.
#
#  INPUTS:
#    MAGIC_ROOT       -- MAGIC installation directory or "no" or empty
#                       * If "no" then do not use MAGIC and do not search for it.
#                       * If empty use MAGIC if found, no error if not found
#                       * Else require MAGIC to exist at specified location and use it
#
#  OUTPUTS:
#    MAGIC_FOOUND     -- Boolean: whether the MAGIC library was found.
#    MAGIC_LIBRARY    -- String: full name of MAGIC library of a string that ends with NOTFOUND
#    MAGIC_LIBRARIES  -- String: names of libraries necessary to use MAGIC

macro(find_magic)
  if("${MAGIC_ROOT}" STREQUAL "no")
    # Do not use MAGIC, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(MAGIC_FOUND FALSE)
    set(MAGIC_LIBRARY "")
    set(MAGIC_LIBRARIES "")

  else()
    # Header files.
    if("${MAGIC_ROOT}" STREQUAL "")
      # no extra include directories necessary
    else()
      include_directories("${MAGIC_ROOT}/include")
    endif()

    # MAGIC library.
    if("${MAGIC_ROOT}" STREQUAL "")
      find_library(MAGIC_LIBRARY NAMES magic)
    else()
      find_library(MAGIC_LIBRARY NAMES magic PATHS "${MAGIC_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
    if(MAGIC_LIBRARY)
      set(MAGIC_FOUND TRUE)
      set(MAGIC_LIBRARIES magic)
    endif()

    # Error if not found?
    if((NOT ("${MAGIC_ROOT}" STREQUAL "")) AND NOT MAGIC_FOUND)
      message(FATAL_ERROR "Magic (libmagic) requested by user at '${MAGIC_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "MAGIC_ROOT       = '${MAGIC_ROOT}'")
    message(STATUS "MAGIC_FOUND      = '${MAGIC_FOUND}'")
    if(MAGIC_FOUND)
      message(STATUS "MAGIC_LIBRARY    = '${MAGIC_LIBRARY}'")
      message(STATUS "MAGIC_LIBRARIES  = '${MAGIC_LIBRARIES}'")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_LIBMAGIC ${MAGIC_FOUND})
endmacro()
