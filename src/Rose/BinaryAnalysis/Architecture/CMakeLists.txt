add_library(roseBinaryAnalysisArchitecture OBJECT
  Base.C
  BasicTypes.C
)

add_dependencies(roseBinaryAnalysisArchitecture rosetta_generated)

install(FIELS
  Base.h
  BasicTypes.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Architecture)

