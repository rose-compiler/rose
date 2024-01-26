add_library(roseSarif OBJECT
  Analysis.C
  Artifact.C
  CodeFlow.C
  Exception.C
  Location.C
  Log.C
  Node.C
  Result.C
  Rule.C
  ThreadFlow.C
  ThreadFlowLocation.C
)

add_dependencies(roseSarif rosetta_generated)

install(FILES
  Analysis.h
  Artifact.h
  BasicTypes.h
  CodeFlow.h
  Exception.h
  Location.h
  Log.h
  Node.h
  Result.h
  Rule.h
  ThreadFlow.h
  ThreadFlowLocation.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Sarif)
