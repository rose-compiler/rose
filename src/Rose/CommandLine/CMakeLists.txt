add_library(roseCommandLine OBJECT
  DurationParser.C License.C Parser.C SelfTest.C Version.C)

install(
  FILES DurationParser.h IntervalParser.h License.h Parser.h SelfTest.h SuffixMultiplierParser.h Version.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/CommandLine)

add_dependencies(roseCommandLine rosetta_generated)
