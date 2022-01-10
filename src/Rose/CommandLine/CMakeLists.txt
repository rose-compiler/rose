add_library(roseCommandLine OBJECT
  CommandLine.C DurationParser.C)

install(
  FILES DurationParser.h IntervalParser.h SuffixMultiplierParser.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/CommandLine)

add_dependencies(roseCommandLine rosetta_generated)
