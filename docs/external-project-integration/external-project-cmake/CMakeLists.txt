####################################################################################################
# This is an example external project that uses ROSE as a dependency.
####################################################################################################

set(PROJECT_NAME RoseExternalExample)

#---------------------------------------------------------------------------------------------------
# This section is mostly boilerplate and will likely be almost idential for every external project
# that depends on ROSE.
#---------------------------------------------------------------------------------------------------

# Check ROSE's top-level CMakeLists.txt. You probably want to use that version or later.
cmake_minimum_required(VERSION 3.15)

# Read the version of this tool from a VERSION file. The file should be one line, and the version
# may be surrounded by double quotes so it can be ineterpolated into a C/C++ program as a string
# literal.
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PROJECT_VERSION)
string(STRIP "${PROJECT_VERSION}" PROJECT_VERSION)
string(REPLACE "\"" "" PROJECT_VERSION "${PROJECT_VERSION}")

# Give your project a name and version number.
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# If your project requiers a compiler later than what was used to compile ROSE, do that here.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ROSE - CMake will search CMAKE_PREFIX_PATH for RoseConfig.cmake
# You can set CMAKE_PREFIX_PATH via:
#   1. Command line: cmake -DCMAKE_PREFIX_PATH=/path/to/rose/install ..
#   2. Environment: export CMAKE_PREFIX_PATH=/path/to/rose/install
#   3. Cache variable: cmake -DRose_DIR=/path/to/rose/install/lib/cmake/Rose ..
find_package(Rose 0.11 REQUIRED)

# Show what we found
message(STATUS "==================================================")
message(STATUS "Found ROSE ${Rose_VERSION} at ${Rose_INSTALL_PREFIX}")
message(STATUS "  Ada:        ${Rose_ENABLE_ADA}")
message(STATUS "  Binaries:   ${Rose_ENABLE_BINARY_ANALYSIS}")
message(STATUS "  C/C++:      ${Rose_ENABLE_C}")
message(STATUS "  Fortran:    ${Rose_ENABLE_FORTRAN}")
message(STATUS "  Java:       ${Rose_ENABLE_JAVA}")
message(STATUS "  Jovial:     ${Rose_ENABLE_JOVIAL}")
message(STATUS "  OpenCL:     ${Rose_ENABLE_OPENCL}")
message(STATUS "  PHP:        ${Rose_ENABLE_PHP}")
message(STATUS "  Python:     ${Rose_ENABLE_PYTHON}")

# If you want to see *all* the ROSE variables, uncomment this code.
#get_cmake_property(_varNames VARIABLES)
#foreach(_varName ${_varNames})
#  if(_varName MATCHES "Rose|ROSE")
#    message(STATUS "${_varName} = ${${_varName}}")
#  endif()
#endforeach()

# Decide where to install your project's files. If no installation prefix was specified, default to
# the ROSE installation location. Doing so is an easy way to have multiple versions and
# configuration of the ROSE library and all tools installed at once. Note that we use this over
# ${Rose_INSTALL_PREFIX} for technical reasons related to how ROSE sometimes gets installed.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    get_target_property(ROSE_LOCATION Rose::rose LOCATION)
    if(ROSE_LOCATION)
        get_filename_component(ROSE_LIB_DIR "${ROSE_LOCATION}" DIRECTORY)
        get_filename_component(ROSE_ROOT "${ROSE_LIB_DIR}" DIRECTORY)
        set(CMAKE_INSTALL_PREFIX "${ROSE_ROOT}" CACHE PATH "Install path prefix, defaults to ROSE installation" FORCE)
        message(STATUS "No install prefix specified, defaulting to ROSE location: ${CMAKE_INSTALL_PREFIX}")
    endif()
endif()

# Cause installed programs to use an internal rpath when searching for dynamic libraries.
#
# Embedding rpath in executables is generally superior to relying on LD_LIBRARY_PATH for several
# important reasons.
#
#   1. The embedded rpath is self-contained within the binary itself, meaning the executable will
#      work correctly regardless of the user's environment or shell configuration—there's no need
#      for users to set environment variables or source setup scripts before running the program.
#
#   2. The embedded rpath provides better security because LD_LIBRARY_PATH affects all executables
#      run from that shell and can be hijacked to load malicious libraries, whereas rpath only
#      affects the specific binary and cannot be overridden by regular users.
#
#   3. The embedded rpath enables easier deployment and distribution since binaries can be relocated
#      (especially with $ORIGIN-relative paths) without requiring installation instructions about
#      environment setup. Finally, rpath avoids version conflicts that commonly occur with
#      LD_LIBRARY_PATH. When multiple applications require different versions of the same library,
#      setting a global LD_LIBRARY_PATH will cause conflicts, but each binary with its own rpath can
#      find exactly the library version it was built against.
#
#   4. The embedded rpath works better for ROSE-based tools that invoke a backend compiler. Using
#      LD_LIBRARY_PATH to accomplish this would pollute the environment for the backend compiler,
#      possibly causing it to link against incorrect libraries resulting in potentially subtle bugs.
#
# The only downside is that rpath is slightly less flexible for quick development testing, but for
# production deployments and user-facing tools, embedded rpath is decisively the better choice.
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#---------------------------------------------------------------------------------------------------
# This part is where you describe what needs to be built and installed for your project. It probably
# consists of your libraries, header files, and executables.  You shouldn't have to mention any of
# ROSE's own dependencies since they get propagated automatically.
# ---------------------------------------------------------------------------------------------------

# Create a simple ROSE-based analyzer
add_executable(simple_analyzer simple_analyzer.cpp)

# Link against ROSE - this automatically:
#   - adds ROSE include directories
#   - links ROSE library and its dependencies
#   - sets C++14 or later standard
#   - configures necessary compile flags
target_link_libraries(simple_analyzer PRIVATE Rose::rose)

# Create an optional binary analysis tool if ROSE was configured with binary analysis capabilities.
if(Rose_ENABLE_BINARY_ANALYSIS)
    add_executable(binary_analyzer binary_analyzer.cpp)
    target_link_libraries(binary_analyzer PRIVATE Rose::rose)

    # You can set preprocessor definitions based on features
    target_compile_definitions(binary_analyzer PRIVATE
        HAVE_ROSE_BINARY_ANALYSIS
    )

    message(STATUS "Building binary_analyzer (ROSE has binary analysis)")
else()
    message(STATUS "Skipping binary_analyzer (ROSE lacks binary analysis)")
endif()

# Create an optional Fortran analysis tool if ROSE was configured with Fortran analysis capabilities.
if(Rose_ENABLE_FORTRAN)
    add_executable(fortran_analyzer fortran_analyzer.cpp)
    target_link_libraries(fortran_analyzer PRIVATE Rose::rose)
    target_compile_definitions(fortran_analyzer PRIVATE
        HAVE_ROSE_FORTRAN
    )
    message(STATUS "Building fortran_analyzer (ROSE has Fortran support)")
else()
    message(STATUS "Skipping fortran_analyzer (ROSE lacks Fortran support)")
endif()

# You probably want to install some of your build artifacts.
install(TARGETS simple_analyzer
    RUNTIME DESTINATION bin
)

if(Rose_ENABLE_BINARY_ANALYSIS)
    install(TARGETS binary_analyzer
        RUNTIME DESTINATION bin
    )
endif()

if(Rose_ENABLE_FORTRAN)
    install(TARGETS fortran_analyzer
        RUNTIME DESTINATION bin
    )
endif()
