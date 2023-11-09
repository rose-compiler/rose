add_library(roseBinaryAnalysisArchitecture OBJECT
  Base.C
  BasicTypes.C
  Exception.C
)

add_dependencies(roseBinaryAnalysisArchitecture rosetta_generated)

install(FIELS
  Base.h
  BasicTypes.h
  Exception.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Architecture)

