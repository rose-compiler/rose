# Spot detection for ROSE.
#
#  INPUTS:
#    ROSE_ROOT       -- ROSE installation directory or "no" or empty
#                       * If "no" then do not use ROSE and do not search for it.
#                       * If empty use ROSE if found, no error if not found
#                       * Else require ROSE to exist at specified location and use it
#
#  OUTPUTS:
#    ROSE_FOUND     -- Boolean: whether the ROSE library was found.

macro(find_rose)
  if("${ROSE_ROOT}" STREQUAL "no")
    # Do not use ROSE, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(ROSE_FOUND FALSE)

  elseif("${ROSE_ROOT}" STREQUAL "")
    message(WARNING "Spot searching is not supported yet; assuming -DROSE_ROOT=no")
    set(ROSE_ROOT no)
    set(ROSE_FOUND FALSE)

  else()
    message(FATAL_ERROR "Spot explicit root is not supported yet; use -DROSE_ROOT=no")
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "ROSE_ROOT       = '${ROSE_ROOT}'")
    message(STATUS "ROSE_FOUND      = '${ROSE_FOUND}'")
  endif()

  # ROSE variables
  set(ROSE_HAVE_ROSE ${ROSE_FOUND})
endmacro()
