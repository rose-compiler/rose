# Readline detection for ROSE.
#
#  INPUTS:
#    READLINE_ROOT       -- READLINE installation directory or "no" or empty
#                       * If "no" then do not use READLINE and do not search for it.
#                       * If empty use READLINE if found, no error if not found
#                       * Else require READLINE to exist at specified location and use it
#
#  OUTPUTS:
#    READLINE_FOOUND     -- Boolean: whether the READLINE library was found.

macro(find_readline)
  if("${READLINE_ROOT}" STREQUAL "no")
    # Do not use READLINE, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(READLINE_FOUND FALSE)

  elseif("${READLINE_ROOT}" STREQUAL "")
    message(WARNING "Readline searching is not supported yet; assuming -DREADLINE_ROOT=no")
    set(READLINE_ROOT no)
    set(READLINE_FOUND FALSE)

  else()
    message(FATAL_ERROR "Readline explicit root is not supported yet; use -DREADLINE_ROOT=no")
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "READLINE_ROOT       = '${READLINE_ROOT}'")
    message(STATUS "READLINE_FOUND      = '${READLINE_FOUND}'")
  endif()

  # ROSE variables
  SET(ROSE_HAVE_LIBREADLINE ${READLINE_FOUND})
endmacro()
