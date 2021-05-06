add_library(roseCommandLine OBJECT
  CommandLine.C)

add_dependencies(roseCommandLine rosetta_generated)
