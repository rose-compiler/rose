add_library(roseBinaryAnalysisArchitecture OBJECT
  Base.C
)

add_dependencies(roseBinaryAnalysisArchitecture rosetta_generated)

install(FIELS
  Base.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Architecture)

