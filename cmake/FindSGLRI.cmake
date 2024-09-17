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
    message(FATAL_ERROR "Jovial support requires stratego. Please define STRATEGO_ROOT")
  endif() 
  
  # Try to find the executable 
  find_program(STRATEGO_EXECUTABLE NAMES sglri PATHS "${STRATEGO_ROOT}/bin" NO_DEFAULT_PATH)

  # Check if we found the executable
  if("${STRATEGO_EXECUTABLE}" STREQUAL "STRATEGO_EXECUTABLE-NOTFOUND")
    message(FATAL_ERROR "The sglri executable was not found") 
  endif()
  
  # Define macro for line 32 of src/frontend/Experimental_Jovial_ROSE_Connection/jovial_support.C
  set(STRATEGO_BIN_PATH "${STRATEGO_ROOT}/bin") 
  # Define macro for cmdline.C
  set(BACKEND_JOVIAL_COMPILER_NAME_WITH_PATH "default_jovial_compiler")
  # Define macro for sage_support.C, SageTreeBuilder.C, and utility_functions.C
  set(ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION 1) 

  # Try to find the sglri executable
  find_program(STRATEGO_SGLRI NAMES sglri PATHS "${STRATEGO_ROOT}/bin" NO_DEFAULT_PATH)

  # Check if we found stratego sglri executable
  if(${STRATEGO_SGLRI} STREQUAL "STRATEGO_SGLRI-NOTFOUND")
    message(STATUS "sglri was not found")
    message(FATAL_ERROR "The Jovial frontend requires the Stratego sglri program\nPlease define STRATEGO_ROOT with location of bin/sglri.")
  endif() 

  # Summarize
  if(VERBOSE)
    message(STATUS "STRATEGO_ROOT       = ${STRATEGO_ROOT}")
    message(STATUS "STRATEGO_EXECUTABLE = ${STRATEGO_EXECUTABLE}")
  endif() 
endmacro()
