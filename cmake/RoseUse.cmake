# RoseUse.cmake - Helper functions for external projects using ROSE
# This file is automatically included by RoseConfig.cmake after find_package(Rose)
# It provides convenience functions to simplify external project CMakeLists.txt files

# rose_setup_build_flags()
#
# Configures build flags to match ROSE's conventions:
# - Removes -DNDEBUG from RelWithDebInfo builds (keeps assertions enabled)
# - Adds -g -O0 to Debug builds for better debugging experience
#
# Usage:
#   find_package(Rose REQUIRED)
#   project(MyProject)
#   rose_setup_build_flags()
#
function(rose_setup_build_flags)
    # Keep assertions enabled in RelWithDebInfo builds (same as ROSE)
    string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" PARENT_SCOPE)

    # Use -g -O0 for debug builds for better debugging
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0" PARENT_SCOPE)
endfunction()

# rose_default_build_type()
#
# Sets CMAKE_BUILD_TYPE to match ROSE's build type if not already set.
# Falls back to RelWithDebInfo if ROSE's build type is not available.
#
# Usage:
#   find_package(Rose REQUIRED)
#   project(MyProject)
#   rose_default_build_type()
#
function(rose_default_build_type)
    if(NOT CMAKE_BUILD_TYPE)
        if(Rose_BUILD_TYPE)
            set(CMAKE_BUILD_TYPE "${Rose_BUILD_TYPE}" CACHE STRING "Build type (inherited from ROSE)" FORCE)
            message(STATUS "No build type specified, defaulting to ROSE's build type: ${CMAKE_BUILD_TYPE}")
        else()
            set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type" FORCE)
            message(STATUS "No build type specified, defaulting to RelWithDebInfo")
        endif()
    endif()
endfunction()

# rose_default_install_prefix()
#
# Sets CMAKE_INSTALL_PREFIX to ROSE's installation directory if not already set.
# This allows external tools to be installed alongside ROSE, making it easy to
# have multiple versions of ROSE and associated tools installed simultaneously.
#
# Usage:
#   find_package(Rose REQUIRED)
#   project(MyProject)
#   rose_default_install_prefix()
#
function(rose_default_install_prefix)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        # Get ROSE's installation directory
        get_target_property(Rose_LOCATION Rose::rose LOCATION)
        if(Rose_LOCATION)
            get_filename_component(Rose_LIB_DIR "${Rose_LOCATION}" DIRECTORY)
            get_filename_component(Rose_ROOT "${Rose_LIB_DIR}" DIRECTORY)
            set(CMAKE_INSTALL_PREFIX "${Rose_ROOT}" CACHE PATH "Install path prefix, defaults to ROSE installation" FORCE)
            message(STATUS "No install prefix specified, defaulting to ROSE location: ${CMAKE_INSTALL_PREFIX}")
        endif()
    endif()
endfunction()

# rose_setup_rpath()
#
# Configures executables to use embedded RPATH for finding shared libraries.
# This is strongly recommended for ROSE-based tools because:
#
#   1. Self-contained: Binaries work without requiring users to set LD_LIBRARY_PATH
#   2. Security: RPATH cannot be hijacked like LD_LIBRARY_PATH
#   3. Distribution: Easier deployment and relocation of binaries
#   4. Compatibility: Works better with ROSE tools that invoke backend compilers
#
# WHY THIS COMPLEXITY IS NECESSARY:
#
# CMake has built-in mechanisms for automatic RPATH handling (CMAKE_INSTALL_RPATH_USE_LINK_PATH),
# but these mechanisms FAIL in ROSE's use case due to several factors:
#
# 1. Same-Prefix Installation Problem:
#    When external projects install to the same prefix as ROSE (common practice to keep
#    all tools together), CMake detects this and STRIPS the RPATH during installation,
#    assuming libraries are in "system" locations. This breaks runtime library resolution.
#    Evidence: cmake_install.cmake contains "file(RPATH_CHANGE ... NEW_RPATH "")"
#
# 2. Mixed Target Types:
#    ROSE's INTERFACE_LINK_LIBRARIES contains both:
#    - CMake targets (Boost::filesystem, dlib::dlib, z3::libz3)
#    - Absolute file paths (/path/to/libcapstone.so, /path/to/libgcrypt.so)
#    CMake's automatic RPATH detection doesn't reliably handle absolute file paths.
#
# 3. Imported Target Property Variations:
#    Imported targets store location in different properties depending on how they
#    were created: IMPORTED_LOCATION, IMPORTED_LOCATION_RELEASE, IMPORTED_LOCATION_DEBUG,
#    or LOCATION. CMake's automatic mechanisms don't consistently traverse all variants.
#
# 4. CMake's Overzealous RPATH Stripping:
#    CMake tries to be "smart" by removing "unnecessary" RPATH entries. This backfires
#    when dependencies are genuinely needed but appear to be in "standard" locations.
#    The stripping occurs even when CMAKE_SKIP_INSTALL_RPATH is FALSE.
#
# OUR SOLUTION:
#
# This function works around these CMake limitations by:
# - Explicitly collecting ALL library directories from ROSE and its dependencies
# - Forcing CMAKE_INSTALL_RPATH as a cache variable (overrides CMake's stripping)
# - Handling both CMake targets AND absolute file paths
# - Checking multiple location properties for imported targets
# - Setting CMAKE_SKIP_INSTALL_RPATH=FALSE to prevent complete RPATH removal
#
# The result: External projects always find ROSE libraries at runtime, regardless of
# installation location, without requiring LD_LIBRARY_PATH.
#
# Usage:
#   find_package(Rose REQUIRED)
#   project(MyProject)
#   rose_setup_rpath()
#
function(rose_setup_rpath)
    # Configure RPATH settings for proper runtime library discovery.
    # These settings ensure that installed executables can find ROSE libraries
    # without requiring LD_LIBRARY_PATH, even when installed to the same prefix as ROSE.

    # Don't skip RPATH for the build tree - enables running executables from build directory
    set(CMAKE_SKIP_BUILD_RPATH FALSE PARENT_SCOPE)

    # Don't skip RPATH when installing - critical for finding ROSE libraries at runtime
    # Without this, CMake may strip RPATH when installing to the same prefix as dependencies
    set(CMAKE_SKIP_INSTALL_RPATH FALSE PARENT_SCOPE)

    # Don't use the install RPATH during building - keeps build and install separate
    # Executables in the build tree use build RPATH, installed executables use install RPATH
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE PARENT_SCOPE)

    # Add directories of linked libraries to the install RPATH automatically
    # This includes directories from all target_link_libraries() calls
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE PARENT_SCOPE)

    # Use $ORIGIN in build RPATH for relocatable builds
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE PARENT_SCOPE)

    # Explicitly add ROSE's library directory and dependency directories to install RPATH.
    # We need to do this explicitly because CMAKE_INSTALL_RPATH_USE_LINK_PATH doesn't work
    # reliably when installing to the same prefix as dependencies - CMake strips the RPATH
    # thinking the libraries are in system/toolchain locations.

    # Collect all library directories we need in RPATH
    set(_rpath_dirs "")

    # Add ROSE's library directory
    get_target_property(Rose_LOCATION Rose::rose LOCATION)
    if(Rose_LOCATION)
        get_filename_component(Rose_LIB_DIR "${Rose_LOCATION}" DIRECTORY)
        list(APPEND _rpath_dirs "${Rose_LIB_DIR}")
    endif()

    # Get library directories from ROSE's dependencies that are in its interface
    # This includes Boost, Capstone, libgcrypt, dlib, z3, and other dependencies
    # These are the directories that were in the build RPATH
    get_target_property(Rose_LINK_LIBRARIES Rose::rose INTERFACE_LINK_LIBRARIES)
    if(Rose_LINK_LIBRARIES)
        foreach(_lib ${Rose_LINK_LIBRARIES})
            set(_lib_location "")

            # Handle CMake targets
            if(TARGET ${_lib})
                # Try multiple properties to get the library location
                # IMPORTED_LOCATION is used for imported targets
                # LOCATION is used for regular targets
                get_target_property(_lib_location ${_lib} IMPORTED_LOCATION)
                if(NOT _lib_location)
                    get_target_property(_lib_location ${_lib} IMPORTED_LOCATION_RELEASE)
                endif()
                if(NOT _lib_location)
                    get_target_property(_lib_location ${_lib} IMPORTED_LOCATION_DEBUG)
                endif()
                if(NOT _lib_location)
                    get_target_property(_lib_location ${_lib} LOCATION)
                endif()
            elseif(IS_ABSOLUTE "${_lib}" AND EXISTS "${_lib}")
                # Handle absolute paths to library files (e.g., /path/to/libfoo.so)
                set(_lib_location "${_lib}")
            endif()

            # If we found a location, extract its directory
            if(_lib_location AND NOT _lib_location MATCHES "NOTFOUND")
                get_filename_component(_lib_dir "${_lib_location}" DIRECTORY)
                list(APPEND _rpath_dirs "${_lib_dir}")
            endif()
        endforeach()
    endif()

    # Remove duplicates and set as cache variable
    if(_rpath_dirs)
        list(REMOVE_DUPLICATES _rpath_dirs)

        # Set as a cache variable (semicolon-separated list) to ensure it persists
        # and isn't stripped by CMake. Using FORCE overrides CMake's automatic RPATH stripping.
        set(CMAKE_INSTALL_RPATH "${_rpath_dirs}" CACHE STRING "Install RPATH for finding ROSE and dependency libraries" FORCE)
    endif()
endfunction()

# rose_read_version_file(output_variable)
#
# Reads a version string from a VERSION file in the project source directory.
# Strips whitespace and surrounding quotes so the version can be used both in
# CMake and interpolated into C/C++ code as a string literal.
#
# Arguments:
#   output_variable - Name of variable to store the version string
#
# Usage:
#   find_package(Rose REQUIRED)
#   rose_read_version_file(MY_VERSION)
#   project(MyProject VERSION ${MY_VERSION})
#
function(rose_read_version_file output_variable)
    set(version_file "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
    if(EXISTS "${version_file}")
        file(READ "${version_file}" version_content)
        string(STRIP "${version_content}" version_content)
        string(REPLACE "\"" "" version_content "${version_content}")
        set(${output_variable} "${version_content}" PARENT_SCOPE)
    else()
        message(WARNING "VERSION file not found at ${version_file}")
        set(${output_variable} "0.0.0" PARENT_SCOPE)
    endif()
endfunction()

# rose_setup_project([SETUP_BUILD_FLAGS] [SETUP_RPATH] [USE_ROSE_BUILD_TYPE] [USE_ROSE_INSTALL_PREFIX])
#
# Convenience function that applies multiple common ROSE project settings.
#
# When called with no arguments, applies all settings (recommended for most projects).
# When called with specific options, applies only those settings.
#
# Options:
#   SETUP_BUILD_FLAGS       - Apply ROSE-compatible build flags
#   SETUP_RPATH            - Enable RPATH for installed executables
#   USE_ROSE_BUILD_TYPE    - Default CMAKE_BUILD_TYPE to ROSE's build type
#   USE_ROSE_INSTALL_PREFIX - Default CMAKE_INSTALL_PREFIX to ROSE's location
#
# Usage:
#   # Apply all settings (recommended)
#   find_package(Rose 0.11 REQUIRED)
#   project(MyProject VERSION 1.0)
#   rose_setup_project()
#
#   # Or apply only specific settings
#   rose_setup_project(SETUP_BUILD_FLAGS USE_ROSE_BUILD_TYPE)
#
#   # Or call individual functions for maximum control
#   rose_setup_build_flags()
#   rose_default_build_type()
#   rose_default_install_prefix()
#   rose_setup_rpath()
#
function(rose_setup_project)
    set(options SETUP_BUILD_FLAGS SETUP_RPATH USE_ROSE_BUILD_TYPE USE_ROSE_INSTALL_PREFIX)
    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    # If no options specified, enable all settings
    if(NOT ARG_SETUP_BUILD_FLAGS AND NOT ARG_SETUP_RPATH AND
       NOT ARG_USE_ROSE_BUILD_TYPE AND NOT ARG_USE_ROSE_INSTALL_PREFIX)
        set(ARG_SETUP_BUILD_FLAGS TRUE)
        set(ARG_SETUP_RPATH TRUE)
        set(ARG_USE_ROSE_BUILD_TYPE TRUE)
        set(ARG_USE_ROSE_INSTALL_PREFIX TRUE)
    endif()

    if(ARG_SETUP_BUILD_FLAGS)
        rose_setup_build_flags()
    endif()

    if(ARG_USE_ROSE_BUILD_TYPE)
        rose_default_build_type()
    endif()

    if(ARG_USE_ROSE_INSTALL_PREFIX)
        rose_default_install_prefix()
    endif()

    if(ARG_SETUP_RPATH)
        rose_setup_rpath()
    endif()
endfunction()

# rose_print_configuration()
#
# Prints information about the found ROSE installation and its enabled features.
# Useful for debugging and verifying that the correct ROSE installation was found.
#
# Usage:
#   find_package(Rose REQUIRED)
#   rose_print_configuration()
#
function(rose_print_configuration)
    # Get ROSE installation directory
    get_target_property(Rose_LOCATION Rose::rose LOCATION)
    if(Rose_LOCATION)
        get_filename_component(Rose_LIB_DIR "${Rose_LOCATION}" DIRECTORY)
        get_filename_component(Rose_ROOT "${Rose_LIB_DIR}" DIRECTORY)
	set(Rose_ROOT "${Rose_ROOT}" PARENT_SCOPE)
    else()
        set(Rose_ROOT "${Rose_INSTALL_PREFIX}" PARENT_SCOPE)
    endif()

    message(STATUS "Found ROSE ${Rose_VERSION} ${Rose_BUILD_TYPE} at ${Rose_ROOT}")
    message(STATUS "  Ada:        ${Rose_ENABLE_ADA}")
    message(STATUS "  Binaries:   ${Rose_ENABLE_BINARY_ANALYSIS}")
    message(STATUS "  C/C++:      ${Rose_ENABLE_C}")
    message(STATUS "  CUDA:       ${Rose_ENABLE_CUDA}")
    message(STATUS "  Fortran:    ${Rose_ENABLE_FORTRAN}")
    message(STATUS "  Java:       ${Rose_ENABLE_JAVA}")
    message(STATUS "  Jovial:     ${Rose_ENABLE_JOVIAL}")
    message(STATUS "  OpenCL:     ${Rose_ENABLE_OPENCL}")
    message(STATUS "  PHP:        ${Rose_ENABLE_PHP}")
    message(STATUS "  Python:     ${Rose_ENABLE_PYTHON}")
endfunction()
