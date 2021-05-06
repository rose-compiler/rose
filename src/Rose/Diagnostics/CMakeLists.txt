add_library(roseDiagnostics OBJECT
  Diagnostics.C)

add_dependencies(roseDiagnostics rosetta_generated)
