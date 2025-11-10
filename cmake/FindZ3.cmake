# Modern Z3 detection for ROSE using Z3's CMake package config files.
#
# This module uses Z3's built-in CMake support (Z3Config.cmake) which is installed
# with modern Z3 versions (4.5.0 and later). This approach is simpler, more reliable,
# and follows modern CMake best practices by using imported targets.
#
# INPUTS:
#   Z3_ROOT       -- Z3 installation directory or "no" or empty
#                    * If "no" then do not use Z3 and do not search for it.
#                    * If empty use Z3 if found, no error if not found
#                    * Else require Z3 to exist at specified location and use it
#
# OUTPUTS:
#   Z3_FOUND       -- Boolean: whether Z3 was found (library and/or executable)
#   Z3_VERSION     -- String: Z3 version string, such as "4.8.1"
#   Z3::libz3      -- Imported target: link against this target to use Z3 library
#   Z3_EXECUTABLE  -- String: path to the z3 executable
#
# ROSE-specific variables (for backward compatibility):
#   ROSE_HAVE_Z3          -- Boolean: whether Z3 library is available
#   ROSE_HAVE_Z3_VERSION_H -- Boolean: whether z3_version.h header exists
#   ROSE_Z3_EXECUTABLE    -- String: path to the z3 executable

macro(find_z3)
  # Ensure CheckIncludeFileCXX module is available (ROSE requires C++)
  include(CheckIncludeFileCXX)

  # This is an attempt to auto-detect whether or not the Z3 lib prefix is lib or lib64. 
  # On RHEL 8, Z3 Cmake is usually at lib/cmake/z3, while on RHEL9, it is lib64/cmake/z3.
  # The key reason is that RHEL9 decides to put libraries at lib64 instead of lib, which can be queried via the CMAKE_INSTALL_LIBDIR variable.
  if (DEFINED ROSE_HOST_OS_IS_RHEL)
    include(GNUInstallDirs)
    set(_Z3_LIB_PREFIX "${CMAKE_INSTALL_LIBDIR}")
  else()
    set(_Z3_LIB_PREFIX "lib")
  endif()

  # Handle Z3_ROOT="no" case: explicitly disable Z3
  if("${Z3_ROOT}" STREQUAL "no")
    set(Z3_FOUND FALSE)
    set(Z3_VERSION "")
    set(Z3_EXECUTABLE "")
    set(ROSE_HAVE_Z3 FALSE)
    set(ROSE_HAVE_Z3_VERSION_H FALSE)
    set(ROSE_Z3_EXECUTABLE "")
    message(STATUS "Z3 support explicitly disabled (Z3_ROOT=no)")

  else()
    # If Z3_ROOT is specified, guide find_package to the right location
    # Z3Config.cmake is typically installed in lib/cmake/z3/ (or lib64/cmake/z3/ on RHEL9)
    if(NOT "${Z3_ROOT}" STREQUAL "")
      set(_Z3_DIR_HINT "${Z3_ROOT}/${_Z3_LIB_PREFIX}/cmake/z3")
      if(NOT EXISTS "${_Z3_DIR_HINT}/Z3Config.cmake")
        message(FATAL_ERROR "Z3 requested at '${Z3_ROOT}' but Z3Config.cmake not found at ${_Z3_DIR_HINT}")
      endif()
      # Set Z3_DIR to guide find_package, but make it a cache variable so users can override
      set(Z3_DIR "${_Z3_DIR_HINT}" CACHE PATH "Path to Z3 CMake config files")
    endif()

    # Find Z3 using its CMake package config (modern approach)
    # Use CONFIG mode to ensure we use Z3Config.cmake, not another FindZ3.cmake
    if(NOT "${Z3_ROOT}" STREQUAL "")
      # If Z3_ROOT specified, require Z3 to be found
      find_package(Z3 CONFIG REQUIRED)
    else()
      # If Z3_ROOT not specified, Z3 is optional
      find_package(Z3 CONFIG QUIET)
    endif()

    # Find Z3 executable (typically not provided by Z3Config.cmake)
    if(NOT "${Z3_ROOT}" STREQUAL "")
      find_program(Z3_EXECUTABLE NAMES z3 PATHS "${Z3_ROOT}/bin" NO_DEFAULT_PATH)
    else()
      find_program(Z3_EXECUTABLE NAMES z3)
    endif()

    # Process results
    if(Z3_FOUND)
      # Z3Config.cmake provides:
      # - Z3_VERSION_STRING (or individual Z3_VERSION_MAJOR, MINOR, PATCH, TWEAK)
      # - z3::libz3 imported target
      # - Z3_C_INCLUDE_DIRS

      # Normalize version variable
      if(DEFINED Z3_VERSION_STRING AND NOT DEFINED Z3_VERSION)
        set(Z3_VERSION "${Z3_VERSION_STRING}")
      endif()

      # Get include directory for header checks
      if(DEFINED Z3_C_INCLUDE_DIRS)
        list(GET Z3_C_INCLUDE_DIRS 0 _Z3_INCLUDE_DIR)
      elseif(TARGET z3::libz3)
        # Fallback: extract from imported target properties
        get_target_property(_Z3_INCLUDE_DIR z3::libz3 INTERFACE_INCLUDE_DIRECTORIES)
      endif()

      # Check for z3_version.h header file
      # z3_version.h is absent from older Z3 versions, and newer versions don't
      # include it automatically in z3.h, so we detect it for conditional compilation
      if(_Z3_INCLUDE_DIR)
        set(CMAKE_REQUIRED_INCLUDES "${_Z3_INCLUDE_DIR}")
        set(CMAKE_REQUIRED_QUIET TRUE)
        check_include_file_cxx("z3_version.h" ROSE_HAVE_Z3_VERSION_H)
      else()
        set(ROSE_HAVE_Z3_VERSION_H FALSE)
      endif()

      # Set ROSE-specific variables
      set(ROSE_HAVE_Z3 TRUE)
      set(ROSE_Z3_EXECUTABLE "${Z3_EXECUTABLE}")

      # Report status
      if(VERBOSE)
        message(STATUS "Z3 library found:")
        message(STATUS "  Z3_VERSION      = ${Z3_VERSION}")
        message(STATUS "  Z3_EXECUTABLE   = ${Z3_EXECUTABLE}")
        message(STATUS "  Z3_INCLUDE_DIRS = ${_Z3_INCLUDE_DIR}")
        message(STATUS "  Z3_VERSION_H    = ${ROSE_HAVE_Z3_VERSION_H}")
        message(STATUS "  Link target     = z3::libz3")
      endif()

    else()
      # Z3 not found
      set(ROSE_HAVE_Z3 FALSE)
      set(ROSE_HAVE_Z3_VERSION_H FALSE)
      set(ROSE_Z3_EXECUTABLE "")

      if(NOT "${Z3_ROOT}" STREQUAL "")
        message(FATAL_ERROR "Z3 requested at '${Z3_ROOT}' but not found")
      endif()

      if(VERBOSE)
        message(STATUS "Z3 not found")
      endif()
    endif()
  endif()

endmacro()
