add_library(roseCommandLine OBJECT
  CommandLine.C)

install(
  FILES IntervalParser.h SuffixMultiplierParser.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/CommandLine)

add_dependencies(roseCommandLine rosetta_generated)
