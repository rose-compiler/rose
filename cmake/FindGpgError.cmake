# Gpg-error detection for ROSE.
#
#  INPUTS:
#    GPGERROR_ROOT       -- Gpg-error installation directory or "no" or empty
#                       * If "no" then do not use Gpg-error and do not search for it.
#                       * If empty use Gpg-error if found, no error if not found
#                       * Else require Gpg-error to exist at specified location and use it
#
#  OUTPUTS:
#    GPGERROR_FOUND        -- Boolean: whether the Gpg-error library was found.
#    GPGERROR_LIBRARY      -- String: full name of Gpg-error library of a string that ends with NOTFOUND
#    GPGERROR_LIBRARY_PATH -- String: directory containing the Gpg-error library
#    GPGERROR_LIBRARIES    -- String: names of libraries necessary to use Gpg-error
#    GPGERROR_HEADER_PATH  -- String: location of gpg-error headers, suitable for including via -I

macro(find_gpgerror)
  if("${GPGERROR_ROOT}" STREQUAL "no")
    # Do not use Gpg-error, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(GPGERROR_FOUND FALSE)
    set(GPGERROR_LIBRARY "")
    set(GPGERROR_LIBRARY_PATH "")
    set(GPGERROR_LIBRARIES "")
    set(GPGERROR_HEADER_PATH "")

  else()
    # Gpg-error library.
    if("${GPGERROR_ROOT}" STREQUAL "")
      find_library(GPGERROR_LIBRARY NAMES gpg-error)
    else()
      find_library(GPGERROR_LIBRARY NAMES gpg-error PATHS "${GPGERROR_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
    if(GPGERROR_LIBRARY)
      set(GPGERROR_FOUND TRUE)
      set(GPGERROR_LIBRARIES gpg-error)
      get_filename_component(GPGERROR_LIBRARY_PATH "${GPGERROR_LIBRARY}" DIRECTORY)
      get_filename_component(GPGERROR_HEADER_PATH "${GPGERROR_LIBRARY_PATH}/../include" ABSOLUTE)
      include_directories("${GPGERROR_HEADER_PATH}")
    endif()

    # Error if not found?
    if((NOT ("${GPGERROR_ROOT}" STREQUAL "")) AND NOT GPGERROR_FOUND)
      message(FATAL_ERROR "Gpg-error requested by user at '${GPGERROR_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "GPGERROR_ROOT         = '${GPGERROR_ROOT}'")
    message(STATUS "GPGERROR_FOUND        = '${GPGERROR_FOUND}'")
    if(GPGERROR_FOUND)
      message(STATUS "GPGERROR_LIBRARY      = '${GPGERROR_LIBRARY}'")
      message(STATUS "GPGERROR_LIBRARY_PATH = '${GPGERROR_LIBRARY_PATH}'")
      message(STATUS "GPGERROR_LIBRARIES    = '${GPGERROR_LIBRARIES}'")
      message(STATUS "GPGERROR_HEADER_PATH  = '${GPGERROR_HEADER_PATH}'")
    endif()
  endif()
endmacro()
