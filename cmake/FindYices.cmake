# Yices detection for ROSE.
# Needs to handle Yices installed in user directories.
# Needs to distinguish between "yices" executable and the library.
#
#  INPUTS:
#    YICES_ROOT       -- Yices installation directory or "no" or empty
#                     * If "no" then do not use Yices and do not search for it.
#                     * If empty use Yices if found, no error if not found
#                     * Else require Yices to exist at specified location and use it
#
#  OUTPUTS:
#    YICES_FOOUND     -- Boolean: whether the yices executable or the Yices library was found.
#    YICES_FOUND_EXE  -- Boolean: whether the yices executable was found
#    YICES_FOUND_LIB  -- Boolean: whether the Yices library was found
#    YICES_EXECUTABLE -- String:  the Yices executable name
#    YICES_VERSION    -- String:  Yices version string, such as "1.2.3"
#    YICES_LIBRARIES  -- String:  names of libraries necessary to use Yices

macro(find_yices)
  if("${YICES_ROOT}" STREQUAL "no")
    # Do not use Yices, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(YICES_FOUND FALSE)
    set(YICES_FOUND_EXE FALSE)
    set(YICES_FOUND_LIB FALSE)
    set(YICES_EXECUTABLE "")
    set(YICES_LIBRARY "")
    set(YICES_VERSION "")
    set(YICES_LIBRARIES "")

  else()
    # Yices executable
    if("${YICES_ROOT}" STREQUAL "")
      find_program(YICES_EXECUTABLE NAMES yices)
    else()
      find_program(YICES_EXECUTABLE NAMES yices PATHS "${YICES_ROOT}/bin" NO_DEFAULT_PATH)
    endif()
    if(YICES_EXECUTABLE)
      set(YICES_FOUND TRUE)
      set(YICES_FOUND_EXE TRUE)
      if(NOT YICES_VERSION)
	execute_process(COMMAND ${YICES_EXECUTABLE} --version OUTPUT_VARIABLE YICES_VERSION_OUTPUT)
	string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" YICES_VERSION "${YICES_VERSION_OUTPUT}")
      endif()
    endif()

    # Yices library.
    if("${YICES_ROOT}" STREQUAL "")
      find_library(YICES_LIBRARY NAMES yices)
    else()
      find_library(YICES_LIBRARY NAMES yices PATHS "${YICES_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
    if(YICES_LIBRARY)
      set(YICES_FOUND TRUE)
      set(YICES_FOUND_LIB TRUE)
      set(YICES_LIBRARIES yices)
      if(NOT YICES_VERSION)
	message(FATAL_ERROR "Yices version cannot be obtained from the library")
      endif()
    endif()

    # Error if not found? [Note: extra(?) parens are due to lack of precedence documentation in cmake]
    if((NOT ("${YICES_ROOT}" STREQUAL "")) AND NOT YICES_FOUND)
      message(FATAL_ERROR "Yices requested by user at '${YICES_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "YICES_ROOT       = '${YICES_ROOT}'")
    message(STATUS "YICES_FOUND      = '${YICES_FOUND}'")
    if(YICES_FOUND)
      message(STATUS "YICES_VERSION    = '${YICES_VERSION}'")
      message(STATUS "YICES_FOUND_EXE  = '${YICES_FOUND_EXE}'")
      message(STATUS "YICES_FOUND_LIB  = '${YICES_FOUND_LIB}'")
      if(YICES_FOUND_EXE)
	message(STATUS "YICES_EXECUTABLE = '${YICES_EXECUTABLE}'")
      endif()
      if(YICES_FOUND_LIB)
	message(STATUS "YICES_LIBRARY    = '${YICES_LIBRARY}'")
	message(STATUS "YICES_LIBRARIES  = '${YICES_LIBRARIES}'")
      endif()
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_LIBYICES ${YICES_FOUND_LIB})
  set(ROSE_YICES ${YICES_EXECUTABLE})
endmacro()
