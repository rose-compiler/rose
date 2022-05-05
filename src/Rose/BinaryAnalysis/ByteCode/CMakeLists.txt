add_library(roseBinaryAnalysisByteCode OBJECT
  Jvm.C
  )

add_dependencies(roseBinaryAnalysisByteCode rosetta_generated)

install(FILES
  ByteCode.h
  Jvm.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/ByteCode)
