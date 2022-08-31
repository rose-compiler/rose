# SGLRI detection for ROSE.
#
#  Note: ROSE was redesigned to just need SGLRI but variables reference STRATEGO for backward
#        compatability.
#
#  INPUTS:
#    STRATEGO_ROOT    -- Stratego installation directory or "no" or empty
#                     * If "no" then do not use Stratego and do not search for it.
#                     * If empty use Stratego if found, no error if not found
#                     * Else require Stratego to exist at specified location and use it
#
#  OUTPUTS:
#    ROSE_WITH_STRATEGO          -- Boolean: Whether Stratego was found.
#    ROSE_WITH_STATREGO_BIN      -- Boolean: Whether Stratego bin was found.
#    ROSE_WITH_STRATEGO_LIB      -- Boolean: Whether Stratego lib was found.
#    STRATEGO_INSTALL_PATH       -- String:  Location of Stratego install.
#    STRATEGO_BIN_PATH           -- String:  Location of Stratego bin.
#    STRATEGO_LIBRARY_PATH       -- String:  Location of Stratego library.
#    STRATEGO_VERSION            -- String:  Stratego Version.
#    USE_ROSE_STRATEGO_SUPPORT   -- Boolean: Use Stratego support.
macro(find_sglri)
  set(STRATEGO_LIBRARY_PATH "")
  set(ROSE_WITH_STRATEGO_LIB FALSE)
  set(USE_ROSE_STRATEGO_SUPPORT FALSE)
  if("${STRATEGO_ROOT}" STREQUAL "no")
    #Do not use Stratego
    set(ROSE_WITH_STRATEGO FALSE)
    set(ROSE_WITH_STRATEGO_BIN FALSE)
    set(STRATEGO_INSTALL_PATH "")
    set(STRATEGO_BIN_PATH "")
    set(STRATEGO_VERSION "")
  else()
    if("${STRATEGO_ROOT}" STREQUAL "")
      find_program(STRATEGO_EXECUTABLE NAMES sglri)
      if(NOT ${STRATEGO_EXECUTABLE} STREQUAL "")
        get_filename_component(STRATEGO_EXE_DIR ${STRATEGO_EXECUTABLE} DIRECTORY)
        set(STRATEGO_ROOT "${STRATEGO_EXE_DIR}/..")
      endif()
    else()
      find_program(STRATEGO_EXECUTABLE NAMES sglri PATHS "${STRATEGO_ROOT}/bin" NO_DEFAULT_PATH)
    endif()

    if(NOT ${STRATEGO_EXECUTABLE} STREQUAL "")
      set(ROSE_WITH_STRATEGO TRUE)
      set(ROSE_WITH_STRATEGO_BIN TRUE)
      get_filename_component(STRATEGO_BIN_PATH ${STRATEGO_EXECUTABLE} DIRECTORY)
    endif()

    if((NOT ("${STRATEGO_ROOT}" STREQUAL "")) AND NOT ROSE_WITH_STRATEGO)
      message(FATAL_ERROR "Stratego requested by user at '${STRATEGO_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "STRATEGO_ROOT       = '${STRATEGO_ROOT}'")
    message(STATUS "ROSE_WITH_STRATEGO  = '${ROSE_WITH_STRATEGO}'")
    if(ROSE_WITH_STRATEGO)
      message(STATUS "STRATEGO_VERSION        = '${STRATEGO_VERSION}'")
      message(STATUS "ROSE_WITH_STRATEGO_BIN  = '${ROSE_WITH_STRATEGO_BIN}'")
      message(STATUS "ROSE_WITH_STRATEGO_LIB  = '${ROSE_WITH_STRATEGO_LIB}'")
      if(ROSE_WITH_STRATEGO_BIN)
	message(STATUS "STRATEGO_EXECUTABLE = '${STRATEGO_EXECUTABLE}'")
      endif()
      if(ROSE_WITH_STRATEGO_LIB)
	message(STATUS "STRATEGO_LIBRARY    = '${STRATEGO_LIBRARY}'")
      endif()
    endif()
  endif()

  # ROSE variables
  set(STRATEGO_INSTALL_PATH ${STRATEGO_ROOT})
  set(STRATEGO_BIN_PATH ${STRATEGO_BIN})
  set(STRATEGO_LIBRARY_PATH ${STRATEGO_LIBRARY})
  set(USE_ROSE_STRATEGO_SUPPORT ${ROSE_WITH_STRATEGO})
endmacro()
