# Wt detection for ROSE.
#
#  INPUTS:
#    WT_ROOT       -- WT installation directory or "no" or empty
#                       * If "no" then do not use WT and do not search for it.
#                       * If empty use WT if found, no error if not found
#                       * Else require WT to exist at specified location and use it
#
#  OUTPUTS:
#    WT_FOOUND     -- Boolean: whether the WT library was found.

macro(find_wt)
  if("${WT_ROOT}" STREQUAL "no")
    # Do not use WT, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(WT_FOUND FALSE)

  elseif("${WT_ROOT}" STREQUAL "")
    message(WARNING "Wt searching is not supported yet; assuming -DWT_ROOT=no")
    set(WT_ROOT no)
    set(WT_FOUND FALSE)

  else()
    message(FATAL_ERROR "Wt explicit root is not supported yet; use -DWT_ROOT=no")
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "WT_ROOT       = '${WT_ROOT}'")
    message(STATUS "WT_FOUND      = '${WT_FOUND}'")
  endif()

  # ROSE variables
  set(ROSE_USE_WT ${WT_FOUND})
endmacro()
