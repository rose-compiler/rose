# Doxygen detection for ROSE.
#
# Inputs:
#    DOXYGEN_ROOT       -- Doxygen installation directory or "no" or empty.
#                          * If "no" then do not use doxygen and do not search for it.
#
# Outputs:
#    DOXYGEN_FOUND      -- Boolean: Whether the doxygen executable was found.
#    DOXYGEN_EXECUTABLE -- String: doxygen executable name
#    DOXYGEN_VERSION    -- String: version string reported by doxygen

macro(find_doxygen)
  if("${DOXYGEN_ROOT}" STREQUAL "no")
    # Do not use doxygen, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(DOXYGEN_FOUND FALSE)
    set(DOXYGEN_EXECUTABLE "")
    set(DOXYGEN_VERSION "")

  else()
    if("${DOXYGEN_ROOT}" STREQUAL "")
      find_program(DOXYGEN_EXECUTABLE NAMES doxygen)
    else()
      find_program(DOXYGEN_EXECUTABLE NAMES doxygen PATHS "${DOXYGEN_ROOT}/bin" NO_DEFAULT_PATH)
    endif()
    if (DOXYGEN_EXECUTABLE)
      set(DOXYGEN_FOUND TRUE)
      execute_process(COMMAND ${DOXYGEN_EXECUTABLE} --version OUTPUT_VARIABLE DOXYGEN_VERSION_OUTPUT)
      string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" DOXYGEN_VERSION "${DOXYGEN_VERSION_OUTPUT}")
    endif()

    # If if not found but requested. Extra(?) parens are due to lack of CMake operaor precedence documentation
    if((NOT ("${DOXYGEN_ROOT}" STREQUAL "")) AND NOT DOXYGEN_FOUND)
      message(FATAL_ERROR "doxygen requested by user at '${DOXYGEN_ROOT}' but not found")
    endif()
  endif()

  # Summarize
  if(VERBOSE)
    message(STATUS "DOXYGEN_ROOT      = '${DOXYGEN_ROOT}'")
    message(STATUS "DOXYGEN_FOUND     = '${DOXYGEN_FOUND}'")
    if(DOXYGEN_FOUND)
      message(STATUS "DOXYGEN_EXECUTABLE = '${DOXYGEN_EXECUTABLE}'")
      message(STATUS "DOXYGEN_VERSION    = '${DOXYGEN_VERSION}'")
    endif()
  endif()

  # ROSE variables
  # (none)
endmacro()
