# Spot detection for ROSE.
#
#  INPUTS:
#    SPOT_ROOT       -- SPOT installation directory or "no" or empty
#                       * If "no" then do not use SPOT and do not search for it.
#                       * If empty use SPOT if found, no error if not found
#                       * Else require SPOT to exist at specified location and use it
#
#  OUTPUTS:
#    SPOT_FOOUND     -- Boolean: whether the SPOT library was found.

macro(find_spot)
  if("${SPOT_ROOT}" STREQUAL "no")
    # Do not use SPOT, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(SPOT_FOUND FALSE)

  elseif("${SPOT_ROOT}" STREQUAL "")
    message(WARNING "Spot searching is not supported yet; assuming -DSPOT_ROOT=no")
    set(SPOT_ROOT no)
    set(SPOT_FOUND FALSE)

  else()
    message(FATAL_ERROR "Spot explicit root is not supported yet; use -DSPOT_ROOT=no")
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "SPOT_ROOT       = '${SPOT_ROOT}'")
    message(STATUS "SPOT_FOUND      = '${SPOT_FOUND}'")
  endif()

  # ROSE variables
  set(ROSE_HAVE_SPOT ${SPOT_FOUND})
endmacro()
