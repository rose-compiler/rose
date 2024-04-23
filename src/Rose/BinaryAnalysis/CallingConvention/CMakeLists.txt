add_library(roseBinaryAnalysisCallingConvention OBJECT
  Analysis.C
  BasicTypes.C
  Declaration.C
  Definition.C
  Exception.C
)

add_dependencies(roseBinaryAnalysisCallingConvention rosetta_generated)

install(FILES
  Analysis.h
  BasicTypes.h
  Declaration.h
  Definition.h
  Exception.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/CallingConvention)
