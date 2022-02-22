# Generate a C string containing the contents of the ROSE_BSD_License.txt file and put the C source code in a file
# named LicenseString.h. LicenseString.h is included by CommandLine/License.C but is not installed.
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/LicenseString.h
  COMMAND perl ${CMAKE_SOURCE_DIR}/scripts/to-c-string.pl <${CMAKE_SOURCE_DIR}/LicenseInformation/ROSE_BSD_License.txt >LicenseString.h
  DEPENDS ${CMAKE_SOURCE_DIR}/LicenseInformation/ROSE_BSD_License.txt
  COMMENT "Generating license C string...")
set_source_files_properties(License.C OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/LicenseString.h)

add_library(roseCommandLine OBJECT
  DurationParser.C License.C Parser.C SelfTest.C Version.C)
add_dependencies(roseCommandLine rosetta_generated)
include_directories(roseCommandLine ${CMAKE_BINARY_DIR}/src)

install(
  FILES DurationParser.h IntervalParser.h License.h Parser.h SelfTest.h SuffixMultiplierParser.h Version.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/CommandLine)

