# Z3 detection for ROSE.
# Needs to support Z3 versions 4.5.0 and later.
# Needs to handle Z3 installed in user directories.
# Needs to distinguish between "z3" executable and the library.
#
#  INPUTS:
#    Z3_ROOT       -- Z3 installation directory or "no" or empty
#                     * If "no" then do not use Z3 and do not search for it.
#                     * If empty use Z3 if found, no error if not found
#                     * Else require Z3 to exist at specified location and use it
#
#  OUTPUTS:
#    Z3_FOUND       -- Boolean: whether the z3 executable or the Z3 library was found.
#    Z3_FOUND_EXE   -- Boolean: whether the z3 executable was found
#    Z3_FOUND_LIB   -- Boolean: whether the Z3 library was found
#    Z3_EXECUTABLE  -- String:  the z3 executable name
#    Z3_VERSION     -- String:  Z3 version string, such as "4.8.1"
#    Z3_VERSION_H   -- Boolean: whether the z3_version.h file exists
#    Z3_LIBRARIES   -- String:  names of libraries necessary to use Z3
#    Z3_HEADER_PATH -- String:  location of z3 headers, suitable for including via -I
include(CheckIncludeFiles) 
macro(find_z3)
  if("${Z3_ROOT}" STREQUAL "no")
    # Do not use Z3, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(Z3_FOUND FALSE)
    set(Z3_FOUND_EXE FALSE)
    set(Z3_FOUND_LIB FALSE)
    set(Z3_EXECUTABLE "")
    set(Z3_LIBRARY "")
    set(Z3_VERSION "")
    set(Z3_VERSION_H FALSE)
    set(Z3_LIBRARIES "")

  else()
    # Z3 executable
    if("${Z3_ROOT}" STREQUAL "")
      find_program(Z3_EXECUTABLE NAMES z3)
    else()
      find_program(Z3_EXECUTABLE NAMES z3 PATHS "${Z3_ROOT}/bin" NO_DEFAULT_PATH)
    endif()
    if(Z3_EXECUTABLE)
      set(Z3_FOUND TRUE)
      set(Z3_FOUND_EXE TRUE)
      if(NOT Z3_VERSION)
	execute_process(COMMAND ${Z3_EXECUTABLE} --version OUTPUT_VARIABLE Z3_VERSION_OUTPUT)
	string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" Z3_VERSION "${Z3_VERSION_OUTPUT}")
      endif()
    endif()

    # Z3 library.
    if("${Z3_ROOT}" STREQUAL "")
      find_library(Z3_LIBRARY NAMES z3)
    else()
      find_library(Z3_LIBRARY NAMES z3 PATHS "${Z3_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
    if(Z3_LIBRARY)
      set(Z3_FOUND TRUE)
      set(Z3_FOUND_LIB TRUE)
      set(Z3_LIBRARIES z3)
      # Newer versions of z3 add an extra directory of indirection, placing headers at include/z3/*.h,
      # so we check if include/z3 is a path and include it if so.
      get_filename_component(Z3_HEADER_PATH ${Z3_LIBRARY} DIRECTORY)
      set(Z3_HEADER_PATH "${Z3_HEADER_PATH}/../include")
      get_filename_component(Z3_HEADER_PATH ${Z3_LIBRARY} DIRECTORY)
      set(Z3_HEADER_PATH "${Z3_HEADER_PATH}/../include")
      if (EXISTS "${Z3_HEADER_PATH}/z3" AND IS_DIRECTORY "${Z3_HEADER_PATH}/z3")
        set(Z3_HEADER_PATH "${Z3_HEADER_PATH}/z3")
      endif()

      include_directories(${Z3_HEADER_PATH})
      
      if(NOT Z3_VERSION)
	message(FATAL_ERROR "Z3 version cannot be obtained from the library")
      endif()
    endif()

    # Error if not found? [Note: extra(?) parens are due to lack of precedence documentation in cmake]
    if((NOT ("${Z3_ROOT}" STREQUAL "")) AND NOT Z3_FOUND)
      message(FATAL_ERROR "Z3 requested by user at '${Z3_ROOT}' but not found")
    endif()

    # Header files. z3_version.h is absent from older versions of Z3, and newer versions don't include it automatically
    # in z3.h, therefore we have to detect it ourselves and conditionally include it in the ROSE source code.
    if("${Z3_ROOT}" STREQUAL "")
      if (Z3_HEADER_PATH)
        set(CMAKE_REQUIRED_INCLUDES ${Z3_HEADER_PATH})
      endif()
      check_include_file("z3_version.h" Z3_VERSION_H)
    else()
      include_directories("${Z3_ROOT}/include")
      check_include_file("${Z3_ROOT}/include/z3_version.h" Z3_VERSION_H)
    endif()
    if(Z3_VERSION_H AND NOT Z3_VERSION)
      message(FATAL_ERROR "Z3 version from z3_version.h not implemented in cmake yet")
    endif()
  endif()

    
  
  # Summarize
  if(VERBOSE)
    message(STATUS "Z3_ROOT       = '${Z3_ROOT}'")
    message(STATUS "Z3_FOUND      = '${Z3_FOUND}'")
    if(Z3_FOUND)
      message(STATUS "Z3_VERSION    = '${Z3_VERSION}'")
      message(STATUS "Z3_FOUND_EXE  = '${Z3_FOUND_EXE}'")
      message(STATUS "Z3_FOUND_LIB  = '${Z3_FOUND_LIB}'")
      if(Z3_FOUND_EXE)
	message(STATUS "Z3_EXECUTABLE = '${Z3_EXECUTABLE}'")
      endif()
      if(Z3_FOUND_LIB)
	message(STATUS "Z3_VERSION_H    = '${Z3_VERSION_H}'")
	message(STATUS "Z3_LIBRARY      = '${Z3_LIBRARY}'")
	message(STATUS "Z3_LIBRARIES    = '${Z3_LIBRARIES}'")
  message(STATUS "Z3_HEADER_PATH  = '${Z3_HEADER_PATH}'")
      endif()
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_Z3 ${Z3_FOUND_LIB})
  set(ROSE_HAVE_Z3_VERSION_H ${Z3_VERSION_H})
  set(ROSE_Z3 ${Z3_EXECUTABLE})
endmacro()
