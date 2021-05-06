add_subdirectory(BinaryAnalysis)
add_subdirectory(Color)
add_subdirectory(CommandLine)
add_subdirectory(Diagnostics)
add_subdirectory(FileSystem)
add_subdirectory(StringUtility)

# needed to properly define ROSE_UTIL_API
add_definitions(-DROSE_UTIL_EXPORTS)

add_library(roseNamespace OBJECT
  Initialize.C
  Progress.C
  SourceLocation.C)

add_dependencies(roseNamespace rosetta_generated)

install(FILES
  BinaryAnalysis.h
  BitFlags.h
  BitOps.h
  Color.h
  CommandLine.h
  Constants.h
  Diagnostics.h
  Exception.h
  FileSystem.h
  FormatRestorer.h
  GraphUtility.h
  Initialize.h
  Progress.h
  RecursionCounter.h
  SourceLocation.h
  StringUtility.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose)
