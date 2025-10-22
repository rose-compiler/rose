# Dlib detection for ROSE using Dlib's native CMake config.
#
# Dlib provides modern CMake package configuration files (dlibConfig.cmake) that create
# imported targets with proper transitive dependencies. This module uses those configs
# instead of manually searching for libraries, which provides better support for both
# static and shared library builds.
#
#  INPUTS:
#    DLIB_ROOT       -- Dlib installation directory or "no" or empty
#                       * If "no" then do not use Dlib and do not search for it
#                       * If empty use Dlib if found, no error if not found
#                       * Else require Dlib to exist at specified location and use it
#
#  Alternative ways to specify dlib location:
#    CMAKE_PREFIX_PATH -- Standard CMake search path (recommended for multiple dependencies)
#                         Example: cmake -DCMAKE_PREFIX_PATH=/path/to/dlib ..
#    dlib_DIR          -- Direct path to Dlib's cmake config directory
#                         Example: cmake -Ddlib_DIR=/path/to/dlib/lib/cmake/dlib ..
#
#  OUTPUTS:
#    DLIB_FOUND      -- Boolean: whether the Dlib library was found
#    dlib::dlib      -- Imported target with all properties (includes, features, dependencies)
#    dlib_VERSION    -- Dlib version string (provided by dlibConfig.cmake)

macro(find_dlib)
  if("${DLIB_ROOT}" STREQUAL "no")
    # Do not use Dlib, and therefore do not even search for it.
    set(DLIB_FOUND FALSE)

  else()
    # Translate DLIB_ROOT to dlib_DIR if provided (for backward compatibility)
    # This allows users to continue using -DDLIB_ROOT=/path/to/dlib
    # Note: We must set dlib_DIR explicitly because CMake policy CMP0144 causes
    # find_package() to ignore <PackageName>_ROOT variables for compatibility.
    if(NOT "${DLIB_ROOT}" STREQUAL "")
      set(dlib_DIR "${DLIB_ROOT}/lib/cmake/dlib")
    endif()

    # Use Dlib's native CMake package config
    # This automatically handles both static and shared libraries, include directories,
    # compile features, and transitive dependencies (like X11 libraries)
    find_package(dlib QUIET CONFIG)
    set(DLIB_FOUND ${dlib_FOUND})

    # ROSE builds shared libraries, which cannot link against static libraries
    # unless they were compiled with -fPIC. Reject static dlib libraries because
    # dlib (at least version 19.19) doesn't use -fPIC to build its static libraries.
    if(DLIB_FOUND AND TARGET dlib::dlib)
      # Query the target directly to check if it's a static library
      get_target_property(DLIB_LOCATION dlib::dlib IMPORTED_LOCATION_RELEASE)
      if(NOT DLIB_LOCATION)
        get_target_property(DLIB_LOCATION dlib::dlib IMPORTED_LOCATION)
      endif()

      if(DLIB_LOCATION AND EXISTS "${DLIB_LOCATION}")
        get_filename_component(DLIB_EXT "${DLIB_LOCATION}" EXT)
        if(DLIB_EXT STREQUAL ".a")
          message(FATAL_ERROR
            "Dlib is a static library (${DLIB_LOCATION}), but ROSE requires a shared library.\n"
            "Static libraries cause linking errors unless built with -fPIC, and dlib doesn't do this.\n"
            "Please install or build dlib as a shared library:\n"
            "  cmake -DBUILD_SHARED_LIBS=ON ...\n"
            "Or point to a different dlib installation using:\n"
            "  -DDLIB_ROOT=/path/to/shared/dlib\n"
            "  -DCMAKE_PREFIX_PATH=/path/to/shared/dlib\n"
            "Or disable dlib support with:\n"
            "  -DDLIB_ROOT=no")
        endif()
      endif()
    endif()

    # Error if explicitly requested but not found
    if(NOT "${DLIB_ROOT}" STREQUAL "" AND NOT DLIB_FOUND)
      message(FATAL_ERROR
        "Dlib requested by user at '${DLIB_ROOT}' but not found.\n"
        "Expected config file at: ${DLIB_ROOT}/lib/cmake/dlib/dlibConfig.cmake\n"
        "Alternative: use -DCMAKE_PREFIX_PATH=${DLIB_ROOT}")
    endif()
  endif()

  # Summarize
  if(VERBOSE)
    message(STATUS "DLIB_ROOT       = '${DLIB_ROOT}'")
    message(STATUS "dlib_DIR        = '${dlib_DIR}'")
    message(STATUS "DLIB_FOUND      = '${DLIB_FOUND}'")
    if(DLIB_FOUND)
      message(STATUS "dlib_VERSION    = '${dlib_VERSION}'")
      message(STATUS "dlib target     = dlib::dlib")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_DLIB ${DLIB_FOUND})
endmacro()
