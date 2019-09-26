# Bison detection for ROSE.
#
#  INPUTS:
#    BISON_ROOT    -- Bison installation directory or "no" or empty
#                     * If "no" then do not use Bison and do not search for it.
#                     * If empty use Bison if found, no error if not found
#                     * Else require Bison to exist at specified location and use it
#  OUTPUTS:
#    BISON_FOOUND     -- Boolean: whether the z3 executable or the Bison library was found.
#    BISON_EXECUTABLE -- String:  the z3 executable name
#    BISON_VERSION    -- String:  Bison version string, such as "4.8.1"
macro(find_bison)
  if("${BISON_ROOT}" STREQUAL "no")
    # Do not use Bison, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(BISON_FOUND FALSE)
    set(BISON_EXECUTABLE "")
    set(BISON_VERSION "")

  else()
    # Bison executable
    if("${BISON_ROOT}" STREQUAL "")
      find_program(BISON_EXECUTABLE NAMES bison)
    else()
      find_program(BISON_EXECUTABLE NAMES bison PATHS "${BISON_ROOT}/bin" NO_DEFAULT_PATH)
    endif()
    if(BISON_EXECUTABLE)
      set(BISON_FOUND TRUE)
      execute_process(COMMAND ${BISON_EXECUTABLE} --version OUTPUT_VARIABLE BISON_VERSION_OUTPUT)
      string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" BISON_VERSION "${BISON_VERSION_OUTPUT}")
    endif()

    # Error if not found? [Note: extra(?) parens are due to lack of precedence documentation in cmake]
    if((NOT ("${BISON_ROOT}" STREQUAL "")) AND NOT BISON_FOUND)
      message(FATAL_ERROR "Bison requested by user at '${BISON_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "BISON_ROOT       = '${BISON_ROOT}'")
    message(STATUS "BISON_FOUND      = '${BISON_FOUND}'")
    if(BISON_FOUND)
      message(STATUS "BISON_EXECUTABLE = '${BISON_EXECUTABLE}'")
      message(STATUS "BISON_VERSION    = '${BISON_VERSION}'")
    endif()
  endif()
endmacro()
