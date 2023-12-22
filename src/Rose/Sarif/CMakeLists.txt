add_library(roseSarif OBJECT
  Analysis.C
  Exception.C
  Location.C
  Log.C
  Node.C
  Result.C
)

add_dependencies(roseSarif rosetta_generated)

install(FILES
  Analysis.h
  BasicTypes.h
  Exception.h
  Location.h
  Log.h
  Node.h
  Result.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Sarif)
