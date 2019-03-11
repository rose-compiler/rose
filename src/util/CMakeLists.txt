######## Generate rose_paths.C #########

# Simplifies variable reference and escape sequence evaluation in the following file generating commands.
cmake_policy(SET CMP0053 NEW)

file(REMOVE ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp)
file(WRITE  ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "\#include <string> \n\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "/* Use the same header file to declare these variables as is used to reference them so that they will be globally available (and not local). */\n")
file(APPEND  ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "\#include \"rose_paths.h\" \n\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "/* These paths will be absolute or relative depending on how the configure script is called (called with an absolute or relative path). */\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_GFORTRAN_PATH          = \"${CMAKE_Fortran_COMPILER}\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_TOP_SRCDIR    = \"${ROSE_TOP_SRC_DIR}\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_TOP_BUILDDIR  = \"${ROSE_TOP_BINARY_DIR}\";\n")
#FIXME: may be a different directory at times. Not sure what the semantics are.
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_PREFIX        = \"${ROSE_TOP_BINARY_DIR}\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_DATADIR       = \"/*deprecated*/\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_BINDIR        = \"/*deprecated*/\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_INCLUDEDIR    = \"/*deprecated*/\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_INFODIR       = \"/*deprecated*/\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_LIBDIR        = \"/*deprecated*/\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_LIBEXECDIR    = \"/*deprecated*/\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_LOCALSTATEDIR = \"/*deprecated*/\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_MANDIR        = \"/*deprecated*/\";\n\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "/* This will always be an absolute path, while paths above are dependent on how the configure script is called \(called with an absolute or relative path\). */\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR = \"${ROSE_TOP_SRC_DIR}\";\n\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "/* Additional interesting data to provide. */\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_CONFIGURE_DATE     = \"@configure_date@\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_BUILD_OS  = \"${CMAKE_HOST_SYSTEM_VERSION}\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_AUTOMAKE_BUILD_CPU = \"${CMAKE_HOST_SYSTEM_PROCESSOR}\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "const std::string ROSE_OFP_VERSION_STRING = \"@enable-ofp-version@\";\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "/* Numeric form of ROSE version -- assuming ROSE version x.y.zL \(where */\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "/* x, y, and z are numbers, and L is a single lowercase letter from a to j\), */\n")
file(APPEND ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp "/* the numeric value is x * 1000000 + y * 10000 + z * 100 + \(L - 'a'\). */\n")

execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C.tmp
    ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.C)

execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${ROSE_TOP_SRC_DIR}/src/util/rose_paths.h
  ${ROSE_TOP_BINARY_DIR}/src/util/rose_paths.h)

# needed to properly define ROSE_UTIL_API
add_definitions(-DROSE_UTIL_EXPORTS)

######## build sub libraries ##########
add_subdirectory(commandlineProcessing)
add_subdirectory(graphs)
add_subdirectory(Sawyer)
add_subdirectory(stringSupport)
add_subdirectory(support)


######## build main library ###########
set(rose_util_src
  ${CMAKE_BINARY_DIR}/src/util/rose_paths.C
  Color.C
  FileSystem.C
  LinearCongruentialGenerator.C
  rose_getline.C
  processSupport.C
  Progress.C
  rose_getline.C
  rose_strtoull.C
)
add_library(util_main OBJECT ${rose_util_src})
add_dependencies(util_main generate_rosePublicConfig)

add_library(roseUtil INTERFACE)
target_sources(roseUtil INTERFACE
  $<TARGET_OBJECTS:util_main>
  $<TARGET_OBJECTS:util_commandlineProcessing>
  $<TARGET_OBJECTS:util_sawyer>
  $<TARGET_OBJECTS:util_stringSupport>
  $<TARGET_OBJECTS:util_support>
  $<TARGET_OBJECTS:util_graphs>)

########### install files ###############
install(FILES
  BitFlags.h Color.h FileSystem.h FormatRestorer.h
  setup.h processSupport.h rose_paths.h
  compilationFileDatabase.h LinearCongruentialGenerator.h
  Map.h Progress.h timing.h rose_getline.h rose_override.h rose_strtoull.h
  roseTraceLib.c ParallelSort.h GraphUtility.h RecursionCounter.h rose_isnan.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
