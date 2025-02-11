# special_toolchain.cmake
#
# This minimal toolchain file lets CMake handle compiler detection.
# It only implements specialized behavior for cross-compiling with GNU
# compilers by extracting a toolchain prefix from the compiler name.
#
# To use this file, invoke CMake with:
#   cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/special_toolchain.cmake <source_dir>

# -- Specialized cross-compiling handling -----------------------------------
if(CMAKE_CROSSCOMPILING AND CMAKE_CXX_COMPILER)
  # Extract the basename (e.g., "arm-elf-g++") from the full compiler path.
  get_filename_component(COMPILER_BASENAME "${CMAKE_CXX_COMPILER}" NAME)
  # If the basename matches a GNU-style toolchain (e.g. "arm-elf-g++"),
  # capture the prefix ("arm-elf-").
  if(COMPILER_BASENAME MATCHES "^(.+-)[gc]\\+\\+(\\.exe)?$")
    set(TOOLCHAIN_PREFIX "${CMAKE_MATCH_1}" CACHE STRING "Toolchain prefix" FORCE)
  endif()

  # If the prefix contains "llvm-", remove it since LLVM uses system binutils.
  if(TOOLCHAIN_PREFIX MATCHES "(.+-)?llvm-$")
    set(TOOLCHAIN_PREFIX "${CMAKE_MATCH_1}" CACHE STRING "Toolchain prefix" FORCE)
  endif()
endif()

# If a toolchain prefix was detected, configure additional tools.
if(DEFINED TOOLCHAIN_PREFIX)
  message(STATUS "Using toolchain prefix: ${TOOLCHAIN_PREFIX}")
  set(CMAKE_AR "${TOOLCHAIN_PREFIX}ar" CACHE FILEPATH "Archiver" FORCE)
  set(CMAKE_RANLIB "${TOOLCHAIN_PREFIX}ranlib" CACHE FILEPATH "Ranlib" FORCE)
endif()
