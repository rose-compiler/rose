add_library(roseBinaryAnalysisConcolicCallback OBJECT
  MemoryExit.C
  MemoryInput.C
  MemoryTime.C
)

add_dependencies(roseBinaryAnalysisConcolicCallback rosetta_generated)

install(FILES
  BasicTypes.h
  MemoryExit.h
  MemoryInput.h
  MemoryTime.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Concolic/Callback)
