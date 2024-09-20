# Doxygen detection for ROSE.
#
# CMake Variables 
#    DOXYGEN_VERSION    -- Version string reported by doxygen executable
#    DOXYGEN_EXECUTABLE -- Doxygen executable name found
# 
# User Configurable CMake Variables
#    DOXYGEN_ROOT       -- Allows user to control which doxygen by defining the path 
#                          to the Doxygen installation directory at config time 
# 
# CMake Variables that get set automatically from calling find_program() 
#    DOXYGEN_FOUND      -- Boolean: Whether the doxygen executable was found.
#
macro(find_doxygen)
  # Try to find Doxygen depending on if/where user specifies
  if("${DOXYGEN_ROOT}" STREQUAL "")
    message(WARNING "DOXYGEN_ROOT is not set. DOXYGEN_EXECUTABLE might not be found.") 
    find_program(DOXYGEN_EXECUTABLE NAMES doxygen)
  else()
    find_program(DOXYGEN_EXECUTABLE NAMES doxygen PATHS "${DOXYGEN_ROOT}/bin" NO_DEFAULT_PATH)
  endif()
  
  # Was Doxygen found? 
  if(NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen was enabled but not found")
  endif()

  # Do we have a Doxygen executable? 
  if(NOT DOXYGEN_EXECUTABLE)
    message(WARNING "DOXYGEN_EXECUTABLE not found in ${DOXYGEN_ROOT}. Please set DOXYGEN_ROOT to the installation path of Doxygen if Doxygen is needed")
  else()
    # Get Doxygen Version from DOXYGEN_EXECUTABLE
    execute_process(COMMAND ${DOXYGEN_EXECUTABLE} --version OUTPUT_VARIABLE DOXYGEN_VERSION_OUTPUT)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" DOXYGEN_VERSION "${DOXYGEN_VERSION_OUTPUT}")
  endif()

  # Summarize
  if(VERBOSE)
    message(STATUS "DOXYGEN_ROOT      = '${DOXYGEN_ROOT}'")
    message(STATUS "DOXYGEN_FOUND     = '${DOXYGEN_FOUND}'")
    message(STATUS "DOXYGEN_EXECUTABLE = '${DOXYGEN_EXECUTABLE}'")
    message(STATUS "DOXYGEN_VERSION    = '${DOXYGEN_VERSION}'")
  endif()
endmacro()
