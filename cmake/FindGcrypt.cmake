# GCrypt detection for ROSE.
#
#  INPUTS:
#    GCRYPT_ROOT        -- GCrypt installation directory or "no" or empty
#                        * If "no" then do not use GCrypt and do not search for it.
#                        * If empty use GCrypt if found, no error if not found
#                        * Else require GCrypt to exist at specified location and use it
#
#  OUTPUTS:
#    GCRYPT_FOUND        -- Boolean: whether the GCrypt library was found.
#    GCRYPT_LIBRARY      -- String: full name of GCrypt library of a string that ends with NOTFOUND
#    GCRYPT_LIBRARY_PATH -- String: directory containing the GCrypt library
#    GCRYPT_LIBRARIES    -- String: names of libraries necessary to use GCrypt
#    GCRYPT_HEADER_PATH  -- String: location of gcrypt headers, suitable for including via -I

macro(find_gcrypt)
  if("${GCRYPT_ROOT}" STREQUAL "no")
    # Do not use GCrypt, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(GCRYPT_FOUND FALSE CACHE BOOL "Whether GCrypt library was found")
    set(GCRYPT_LIBRARY "")
    set(GCRYPT_LIBRARY_PATH "")
    set(GCRYPT_LIBRARIES "")
    set(GCRYPT_HEADER_PATH "")

  else()
    # GCrypt library.
    if("${GCRYPT_ROOT}" STREQUAL "")
      find_library(GCRYPT_LIBRARY NAMES gcrypt)
    else()
      find_library(GCRYPT_LIBRARY NAMES gcrypt PATHS "${GCRYPT_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
    if(GCRYPT_LIBRARY)
      set(GCRYPT_FOUND TRUE CACHE BOOL "Whether GCrypt library was found")
      set(GCRYPT_LIBRARIES gcrypt)
      get_filename_component(GCRYPT_LIBRARY_PATH "${GCRYPT_LIBRARY}" DIRECTORY)
      get_filename_component(GCRYPT_HEADER_PATH "${GCRYPT_LIBRARY_PATH}/../include" ABSOLUTE)
      include_directories("${GCRYPT_HEADER_PATH}")
    endif()

    # Error if not found?
    if((NOT ("${GCRYPT_ROOT}" STREQUAL "")) AND NOT GCRYPT_FOUND)
      message(FATAL_ERROR "GCrypt requested by user at '${GCRYPT_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "GCRYPT_ROOT         = '${GCRYPT_ROOT}'")
    message(STATUS "GCRYPT_FOUND        = '${GCRYPT_FOUND}'")
    if(GCRYPT_FOUND)
      message(STATUS "GCRYPT_LIBRARY      = '${GCRYPT_LIBRARY}'")
      message(STATUS "GCRYPT_LIBRARY_PATH = '${GCRYPT_LIBRARY_PATH}'")
      message(STATUS "GCRYPT_LIBRARIES    = '${GCRYPT_LIBRARIES}'")
      message(STATUS "GCRYPT_HEADER_PATH  = '${GCRYPT_HEADER_PATH}'")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_LIBGCRYPT ${GCRYPT_FOUND})
endmacro()
