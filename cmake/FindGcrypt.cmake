# GCrypt detection for ROSE.
#
#  INPUTS:
#    GCRYPT_ROOT       -- GCrypt installation directory or "no" or empty
#                       * If "no" then do not use GCrypt and do not search for it.
#                       * If empty use GCrypt if found, no error if not found
#                       * Else require GCrypt to exist at specified location and use it
#
#  OUTPUTS:
#    GCRYPT_FOOUND     -- Boolean: whether the GCrypt library was found.
#    GCRYPT_LIBRARY    -- String: full name of GCrypt library of a string that ends with NOTFOUND
#    GCRYPT_LIBRARIES  -- String: names of libraries necessary to use GCrypt

macro(find_gcrypt)
  if("${GCRYPT_ROOT}" STREQUAL "no")
    # Do not use GCrypt, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(GCRYPT_FOUND FALSE)
    set(GCRYPT_LIBRARY "")
    set(GCRYPT_LIBRARIES "")

  else()
    # Header files.
    if("${GCRYPT_ROOT}" STREQUAL "")
      # no extra include directories necessary
    else()
      include_directories("${GCRYPT_ROOT}/include")
    endif()

    # GCrypt library.
    if("${GCRYPT_ROOT}" STREQUAL "")
      find_library(GCRYPT_LIBRARY NAMES gcrypt)
    else()
      find_library(GCRYPT_LIBRARY NAMES gcrypt PATHS "${GCRYPT_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
    if(GCRYPT_LIBRARY)
      set(GCRYPT_FOUND TRUE)
      set(GCRYPT_LIBRARIES gcrypt)
    endif()

    # Error if not found?
    if((NOT ("${GCRYPT_ROOT}" STREQUAL "")) AND NOT GCRYPT_FOUND)
      message(FATAL_ERROR "GCrypt requested by user at '${GCRYPT_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "GCRYPT_ROOT       = '${GCRYPT_ROOT}'")
    message(STATUS "GCRYPT_FOUND      = '${GCRYPT_FOUND}'")
    if(GCRYPT_FOUND)
      message(STATUS "GCRYPT_LIBRARY    = '${GCRYPT_LIBRARY}'")
      message(STATUS "GCRYPT_LIBRARIES  = '${GCRYPT_LIBRARIES}'")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_LIBGCRYPT ${GCRYPT_FOUND})
endmacro()
