# ATerm detection for ROSE.
#
#  INPUTS:
#    ATERM_ROOT    -- ATerm installation directory or "no" or empty
#                     * If "no" then do not use ATerm and do not search for it.
#                     * If empty use ATerm if found, no error if not found
#                     * Else require ATerm to exist at specified location and use it
#
#  OUTPUTS:
#    ROSE_WITH_ATERM          -- Boolean: Whether ATerm was found.
#    ROSE_WITH_ATERM_BIN      -- Boolean: Whether ATerm bin was found.
#    ROSE_WITH_ATERM_LIB      -- Boolean: Whether ATerm lib was found.
#    ROSE_BUILD_ATERM_SUPPORT -- Boolean: Whether ROSE ATerm support should be built.
#    ATERM_INSTALL_PATH       -- String:  Location of ATerm install.
#    ATERM_BIN_PATH           -- String:  Location of ATerm bin.
#    ATERM_LIBRARY_PATH       -- String:  Location of ATerm library.
#    ATERM_VERSION            -- String:  ATerm Version.
#    USE_ROSE_ATERM_SUPPORT   -- Boolean: Use ATerm support.
macro(find_aterm)
  set(USE_ROSE_ATERM_SUPPORT FALSE)
  if("${ATERM_ROOT}" STREQUAL "no")
    message("Aterm root is no")
    #Do not use ATerm
    set(ROSE_WITH_ATERM FALSE)
    set(ROSE_WITH_ATERM_BIN FALSE)
    set(ROSE_WITH_ATERM_LIB FALSE)
    set(ROSE_BUILD_ATERM_SUPPORT FALSE)
    set(ATERM_INSTALL_PATH "")
    set(ATERM_BIN_PATH "")
    set(ATERM_LIBRARY_PATH "")
    set(ATERM_VERSION "")
  else()
    if("${ATERM_ROOT}" STREQUAL "")
      message("Aterm root is empty")
      find_program(ATERM_EXECUTABLE NAMES atrmannos)
      if(NOT ${ATERM_EXECUTABLE} STREQUAL "ATERM_EXECUTABLE-NOTFOUND")
        get_filename_component(ATERM_EXE_DIR ${ATERM_EXECUTABLE} DIRECTORY)
        set(ATERM_ROOT "${ATERM_EXE_DIR}/..")
      endif()
    else()
      find_program(ATERM_EXECUTABLE NAMES atrmannos PATHS "${ATERM_ROOT}/bin" NO_DEFAULT_PATH)
    endif()

    if(NOT ${ATERM_EXECUTABLE} STREQUAL "ATERM_EXECUTABLE-NOTFOUND")
      set(ROSE_WITH_ATERM TRUE)
      set(ROSE_WITH_ATERM_BIN TRUE)
      get_filename_component(ATERM_BIN_PATH ${ATERM_EXECUTABLE} DIRECTORY)
      if(NOT ATERM_VERSION)
	execute_process(COMMAND ${ATERM_EXECUTABLE} -v OUTPUT_VARIABLE ATERM_VERSION_OUTPUT ERROR_VARIABLE ATERM_VERSION_ERROR)
	string(REGEX MATCH ": [0-9]+\\.[0-9]+" ATERM_VERSION_TEMP "${ATERM_VERSION_ERROR}")
	string(REGEX MATCH "[0-9]+\\.[0-9]+" ATERM_VERSION "${ATERM_VERSION_TEMP}")
      endif()
    endif()

    # ATerm library.
    if("${ATERM_ROOT}" STREQUAL "")
      find_library(ATERM_LIBRARY NAMES ATerm)
    else()
      find_library(ATERM_LIBRARY NAMES ATerm PATHS "${ATERM_ROOT}/lib" NO_DEFAULT_PATH)
    endif()

    if(NOT ${ATERM_LIBRARY} STREQUAL "ATERM_LIBRARY-NOTFOUND")
      get_filename_component(ATERM_LIBRARY_PATH ${ATERM_LIBRARY} DIRECTORY)
      set(ROSE_WITH_ATERM TRUE)
      set(ROSE_WITH_ATERM_LIB TRUE)
    endif()

    if("${ATERM_VERSION}" STREQUAL "")
       set(ATERM_VERSION "Unknown")
    endif()

    if((NOT ("${ATERM_ROOT}" STREQUAL "")) AND NOT ROSE_WITH_ATERM)
      message(FATAL_ERROR "ATerm requested by user at '${ATERM_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "ATERM_ROOT       = '${ATERM_ROOT}'")
    message(STATUS "ROSE_WITH_ATERM  = '${ROSE_WITH_ATERM}'")
    if(ROSE_WITH_ATERM)
      message(STATUS "ATERM_VERSION        = '${ATERM_VERSION}'")
      message(STATUS "ROSE_WITH_ATERM_BIN  = '${ROSE_WITH_ATERM_BIN}'")
      message(STATUS "ROSE_WITH_ATERM_LIB  = '${ROSE_WITH_ATERM_LIB}'")
      if(ROSE_WITH_ATERM_BIN)
	message(STATUS "ATERM_EXECUTABLE = '${ATERM_EXECUTABLE}'")
      endif()
      if(ROSE_WITH_ATERM_LIB)
	message(STATUS "ATERM_LIBRARY    = '${ATERM_LIBRARY}'")
      endif()
    endif()
  endif()

  # ROSE variables
  set(ATERM_INSTALL_PATH ${ATERM_ROOT})
  set(USE_ROSE_ATERM_SUPPORT ${ROSE_WITH_ATERM})
  set(ROSE_BUILD_ATERM_SUPPORT ${ROSE_WITH_ATERM})
endmacro()
