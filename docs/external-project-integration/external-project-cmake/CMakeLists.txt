####################################################################################################
# This is an example external project that uses ROSE as a dependency.
####################################################################################################

set(PROJECT_NAME RoseExternalExample)

#---------------------------------------------------------------------------------------------------
# Boilerplate for finding and using ROSE. Copy this part to your external project.
#---------------------------------------------------------------------------------------------------

# Check ROSE's top-level CMakeLists.txt. You probably want to use that version or later.
cmake_minimum_required(VERSION 3.15)

# Enable C++ language before finding ROSE (required for ROSE's dependency detection)
enable_language(CXX)

# Find ROSE - CMake will search CMAKE_PREFIX_PATH for RoseConfig.cmake
# You can set CMAKE_PREFIX_PATH via:
#   1. Command line: cmake -DCMAKE_PREFIX_PATH=/path/to/rose/install ..
#   2. Environment: export CMAKE_PREFIX_PATH=/path/to/rose/install
#   3. Cache variable: cmake -DRose_DIR=/path/to/rose/install/lib/cmake/Rose ..
find_package(Rose 0.11 REQUIRED)

# Read the version of this tool from a VERSION file
rose_read_version_file(PROJECT_VERSION)

# Give your project a name and version number
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# If your project requires a C++ standard later than what was used to compile ROSE, set it here.
# For example, to require C++17:
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional: Configure project defaults to match ROSE (applies all settings when called with no arguments)
# Individual functions can be called for more control:
#   rose_setup_build_flags()       - Remove -DNDEBUG from RelWithDebInfo, add -g -O0 to Debug
#   rose_default_build_type()      - Use ROSE's build type if CMAKE_BUILD_TYPE not set
#   rose_default_install_prefix()  - Install alongside ROSE by default
#   rose_setup_rpath()             - Use RPATH instead of LD_LIBRARY_PATH
rose_setup_project()

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
