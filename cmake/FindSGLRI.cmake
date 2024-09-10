# SGLRI detection for ROSE.
#
#  Note: ROSE was redesigned to just need SGLRI but variables reference STRATEGO for backward
#        compatability.
#
#    STRATEGO_ROOT    -- Stratego installation directory or "no" or empty
#                     * If "no" then do not use Stratego and do not search for it.
#                     * If empty use Stratego if found, no error if not found
#                     * Else require Stratego to exist at specified location and use it
macro(find_sglri)
  if("${STRATEGO_ROOT}" STREQUAL "no" OR "${STRATEGO_ROOT}" STREQUAL "")
    # Fail because Jovial requires stratego
    message(FATAL_ERROR "Jovial Support requires stratego. Please define STRATEGO_ROOT")
    set(USE_ROSE_STRATEGO_SUPPORT FALSE)
    set(ROSE_WITH_STRATEGO FALSE)
  endif() 
  
  # Try to find the executable 
  find_program(STRATEGO_EXECUTABLE NAMES sglri PATHS "${STRATEGO_ROOT}/bin" NO_DEFAULT_PATH)

  # Check if we found the executable
  if("${STRATEGO_EXECUTABLE}" STREQUAL "STRATEGO_EXECUTABLE-NOTFOUND")
    message(FATAL_ERROR "The sglri executable was not found") 
  endif()

  # Try to find the library 
  find_library(STRATEGO_LIBRARY NAMES ATerm PATHS "${STRATEGO_ROOT}/lib" NO_DEFAULT_PATH)
  
  # Check if we found the library
  if(${STRATEGO_LIBRARY} STREQUAL "STRATEGO_LIBRARY-NOTFOUND")
    message(FATAL_ERROR "libAterm was not found") 
  endif() 
  
  # Finalize -- librose will use this var to link to stratego library in src/CMakleLists.txt  
  set(ROSE_WITH_STRATEGO TRUE)

  # Summarize
  if(VERBOSE)
    message(STATUS "STRATEGO_ROOT       = ${STRATEGO_ROOT}")
    message(STATUS "ROSE_WITH_STRATEGO  = ${ROSE_WITH_STRATEGO}")
	  message(STATUS "STRATEGO_EXECUTABLE = ${STRATEGO_EXECUTABLE}")
    message(STATUS "STRATEGO_LIBRARY    = ${STRATEGO_LIBRARY}")
  endif() 
endmacro()
