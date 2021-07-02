# Readline detection for ROSE.
#
#  INPUTS:
#    READLINE_ROOT       -- READLINE installation directory or "no" or empty
#                       * If "no" then do not use READLINE and do not search for it.
#                       * If empty use READLINE if found, no error if not found
#                       * Else require READLINE to exist at specified location and use it
#
#  OUTPUTS:
#    READLINE_FOUND      -- Boolean: whether the READLINE library was found.
#    READLINE_LIBRARY    -- String: full name of READLINE library of a string that ends with NOTFOUND
#    READLINE_LIBRARIES  -- String: names of libraries necessary to use READLINE
#    READLINE_INCLUDE_DIR-- String: location of include files


macro(find_readline)
  if("${READLINE_ROOT}" STREQUAL "no")
    # Do not use READLINE, and therefore do not even search for it. Make sure all outputs are cleared
    # to avoid problems with users maybe setting them.
    set(READLINE_FOUND FALSE)
    set(READLINE_LIBRARY "")
    set(READLINE_INCLUDE_DIR "")
    set(READLINE_LIBRARIES "")
  else()
    # Header files.
    if("${READLINE_ROOT}" STREQUAL "")
      # no extra include directories necessary
    else()
      include_directories("${READLINE_ROOT}/include")
    endif()

    # READLINE library.
    if("${READLINE_ROOT}" STREQUAL "")
      find_library(READLINE_LIBRARY NAMES readline)
    else()
      find_library(READLINE_LIBRARY NAMES readline PATHS "${READLINE_ROOT}/lib" NO_DEFAULT_PATH)
    endif()

    if(READLINE_LIBRARY)
      set(READLINE_FOUND TRUE)
    endif()

    # Search for include directory
    find_path(READLINE_INCLUDE_DIR
      NAMES readline/readline.h
      HINTS ${READLINE_ROOT}/include
      )

    # Error if not found?
    if((NOT ("${READLINE_ROOT}" STREQUAL "")) AND NOT READLINE_FOUND)
      message(FATAL_ERROR "READLINE requested by user at '${READLINE_ROOT}' but not found")
    endif()
  endif()

  # Summarize
  if(VERBOSE)
    message(STATUS "READLINE_ROOT        = '${READLINE_ROOT}'")
    message(STATUS "READLINE_FOUND       = '${READLINE_FOUND}'")
    if(READLINE_FOUND)
      message(STATUS "READLINE_LIBRARY     = '${READLINE_LIBRARY}'")
      message(STATUS "READLINE_INCLUDE_DIR = '${READLINE_INCLUDE_DIR}'")
      message(STATUS "READLINE_LIBRARIES   = '${READLINE_LIBRARIES}'")
    endif()
  else()
    if(READLINE_FOUND)
      message(STATUS "Found Readline: ${READLINE_INCLUDE_DIR} ${READLINE_LIBRARY}")
    elseif(NOT "${READLINE_ROOT}" STREQUAL "no")
      message(STATUS "Readline NOT found.")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_LIBREADLINE ${READLINE_FOUND})
endmacro()
