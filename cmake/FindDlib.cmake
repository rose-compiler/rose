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
#
#  LEGACY OUTPUTS (for backward compatibility):
#    DLIB_LIBRARY    -- Path to dlib library (deprecated, use dlib::dlib target instead)
#    DLIB_LIBRARIES  -- Library to link (deprecated, use dlib::dlib target instead)

macro(find_dlib)
  if("${DLIB_ROOT}" STREQUAL "no")
    # Do not use Dlib, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(DLIB_FOUND FALSE)
    set(DLIB_LIBRARY "")
    set(DLIB_LIBRARIES "")

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

    # Set legacy variables for backward compatibility
    if(DLIB_FOUND AND TARGET dlib::dlib)
      # Get the actual library location for legacy DLIB_LIBRARY variable
      get_target_property(DLIB_LIBRARY dlib::dlib IMPORTED_LOCATION_RELEASE)
      if(NOT DLIB_LIBRARY)
        get_target_property(DLIB_LIBRARY dlib::dlib IMPORTED_LOCATION)
      endif()
      # For legacy DLIB_LIBRARIES, use the target name (preferred) or library path
      set(DLIB_LIBRARIES dlib::dlib)

      # ROSE builds shared libraries, which cannot link against static libraries
      # unless they were compiled with -fPIC. Reject static dlib libraries.
      if(DLIB_LIBRARY AND EXISTS "${DLIB_LIBRARY}")
        get_filename_component(DLIB_EXT "${DLIB_LIBRARY}" EXT)
        if(DLIB_EXT STREQUAL ".a")
          message(FATAL_ERROR
            "Dlib is a static library (${DLIB_LIBRARY}), but ROSE requires a shared library.\n"
            "Static libraries cause linking errors unless built with -fPIC, and ROSE is built as a shared library.\n"
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
      message(STATUS "DLIB_LIBRARY    = '${DLIB_LIBRARY}' (legacy)")
      message(STATUS "DLIB_LIBRARIES  = '${DLIB_LIBRARIES}' (legacy)")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_DLIB ${DLIB_FOUND})
endmacro()
