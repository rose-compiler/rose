add_subdirectory(BinaryAnalysis)
add_subdirectory(Color)
add_subdirectory(FileSystem)
add_subdirectory(StringUtility)

# needed to properly define ROSE_UTIL_API
add_definitions(-DROSE_UTIL_EXPORTS)

add_library(roseNamespace OBJECT
  Progress.C)

add_dependencies(roseNamespace rosetta_generated)

install(FILES
  BinaryAnalysis.h
  BitFlags.h
  BitOps.h
  Color.h
  Constants.h
  FileSystem.h
  FormatRestorer.h
  GraphUtility.h
  Progress.h
  RecursionCounter.h
  StringUtility.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose)
