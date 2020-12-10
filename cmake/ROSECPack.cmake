# -------------------------------------------------------------------------
# Build a CPack installer
# -------------------------------------------------------------------------

set(CPACK_PACKAGE_NAME "ROSE")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ROSE is an open source compiler infrastructure to build source-to-source program transformation and analysis tools for large-scale applications.")

set(CPACK_PACKAGE_VENDOR "Lawrence Livermore National Laboratory")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/README.md
  ${CMAKE_CURRENT_BINARY_DIR}/README.txt
  @ONLY)
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_BINARY_DIR}/README.txt")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/COPYRIGHT
  ${CMAKE_CURRENT_BINARY_DIR}/COPYRIGHT.txt
  @ONLY)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/COPYRIGHT.txt")

# TODO: write something for these files:
# set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/NOTICE.txt")
# set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/ABOUT.txt")

# TODO: find or define sensible version numbers for ROSE
# set(CPACK_PACKAGE_VERSION_MAJOR "${ROSE_MAJOR_VERSION}")
# set(CPACK_PACKAGE_VERSION_MINOR "${ROSE_MINOR_VERSION}")
# set(CPACK_PACKAGE_VERSION_PATCH "${ROSE_BUILD_VERSION}")
# set(CPACK_PACKAGE_INSTALL_DIRECTORY "ROSE ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
# set(CPACK_SOURCE_PACKAGE_FILE_NAME "ROSE-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "ROSE")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "ROSE")

set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ROSECPackOptions.cmake")

if(APPLE)
  set(CPACK_PACKAGING_INSTALL_PREFIX "/Applications/ROSE")
else()
  set(CPACK_PACKAGING_INSTALL_PREFIX "/")
endif()

# -------------------------------------------------------------------------
# Install ROSE
# -------------------------------------------------------------------------
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};ROSE;ALL;/")

include (InstallRequiredSystemLibraries)
include(CPack)
