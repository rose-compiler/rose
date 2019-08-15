# YAML-cpp detection for ROSE.
#
#  INPUTS:
#    YAMLCPP_ROOT    -- YAMLCPP installation directory or "no" or empty
#                     * If "no" then do not use YAMLCPP and do not search for it.
#                     * If empty use YAMLCPP if found, no error if not found
#                     * Else require YAMLCPP to exist at specified location and use it
#
#  OUTPUTS:
#    YAMLCPP_FOOUND     -- Boolean: whether the YAMLCPP library was found.
#    YAMLCPP_LIBRARY    -- String: full name of YAMLCPP library of a string that ends with NOTFOUND
#    YAMLCPP_LIBRARIES  -- String: names of libraries necessary to use YAMLCPP

macro(find_yamlcpp)
  if("${YAMLCPP_ROOT}" STREQUAL "no")
    # Do not use YAMLCPP, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(YAMLCPP_FOUND FALSE)
    set(YAMLCPP_LIBRARY "")
    set(YAMLCPP_LIBRARIES "")

  else()
    # Header files.
    if("${YAMLCPP_ROOT}" STREQUAL "")
      # no extra include directories necessary
    else()
      include_directories("${YAMLCPP_ROOT}/include")
    endif()

    # YAMLCPP library.
    if("${YAMLCPP_ROOT}" STREQUAL "")
      find_library(YAMLCPP_LIBRARY NAMES yaml-cpp)
    else()
      find_library(YAMLCPP_LIBRARY NAMES yaml-cpp PATHS ${YAMLCPP_ROOT}/lib NO_DEFAULT_PATH)
    endif()
    if(YAMLCPP_LIBRARY)
      set(YAMLCPP_FOUND TRUE)
      set(YAMLCPP_LIBRARIES yaml-cpp)
    endif()

    # Error if not found?
    if((NOT ("${YAMLCPP_ROOT}" STREQUAL "")) AND NOT YAMLCPP_FOUND)
      message(FATAL_ERROR "YAML-CPP requested by user at '${YAMLCPP_ROOT}' but not found")
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "YAMLCPP_ROOT       = '${YAMLCPP_ROOT}'")
    message(STATUS "YAMLCPP_FOUND      = '${YAMLCPP_FOUND}'")
    if(YAMLCPP_FOUND)
      message(STATUS "YAMLCPP_LIBRARY    = '${YAMLCPP_LIBRARY}'")
      message(STATUS "YAMLCPP_LIBRARIES  = '${YAMLCPP_LIBRARIES}'")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_LIBYAML ${YAMLCPP_FOUND})
endmacro()
