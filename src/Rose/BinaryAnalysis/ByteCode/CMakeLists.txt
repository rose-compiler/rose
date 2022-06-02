add_library(roseBinaryAnalysisByteCode OBJECT
  ByteCode.C
  Jvm.C
  )

add_dependencies(roseBinaryAnalysisByteCode rosetta_generated)

install(FILES
  ByteCode.h
  Jvm.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/ByteCode)
