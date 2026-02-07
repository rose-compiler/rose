# UseLocalEDGBinary.cmake
# This module sets up build rules to use EDG binary tarballs from the source tree
# instead of downloading them from the server.

# Create the output directory
set(EDG_BUILD_DIR "${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend")
file(MAKE_DIRECTORY "${EDG_BUILD_DIR}")

# Get the tarball filename without path
get_filename_component(EDG_TARBALL_NAME "${EDG_SOURCE_TARBALL}" NAME)

# Get the directory name (tarball without .tar.gz)
string(REGEX REPLACE "\\.tar\\.gz$" "" EDG_EXTRACTED_DIR "${EDG_TARBALL_NAME}")

message(STATUS "EDG - will extract ${EDG_TARBALL_NAME} at build time")

# Create a custom target to extract the EDG binary
add_custom_target(extract_EDG_binary
  COMMENT "Extracting EDG binary from source tree: ${EDG_TARBALL_NAME}"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${EDG_SOURCE_TARBALL}" "${EDG_BUILD_DIR}/${EDG_TARBALL_NAME}"
  COMMAND ${CMAKE_COMMAND} -E tar xzf "${EDG_BUILD_DIR}/${EDG_TARBALL_NAME}"
  COMMAND ${CMAKE_COMMAND} -E remove_directory "${EDG_BUILD_DIR}/EDG"
  COMMAND ${CMAKE_COMMAND} -E rename "${EDG_BUILD_DIR}/${EDG_EXTRACTED_DIR}" "${EDG_BUILD_DIR}/EDG"
  COMMAND ${CMAKE_COMMAND} -E touch "${EDG_BUILD_DIR}/EDG/libroseEDG.la"
  WORKING_DIRECTORY "${EDG_BUILD_DIR}"
  DEPENDS rosetta_generated
)

# Create the imported library target
add_library(EDG STATIC IMPORTED)
add_dependencies(EDG extract_EDG_binary)
set_property(TARGET EDG PROPERTY IMPORTED_LOCATION
  "${EDG_BUILD_DIR}/EDG/.libs/libroseEDG.a")

# Also create EDG.tar.gz symlink for compatibility
add_custom_command(TARGET extract_EDG_binary POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink "${EDG_TARBALL_NAME}" "EDG.tar.gz"
  WORKING_DIRECTORY "${EDG_BUILD_DIR}"
)
