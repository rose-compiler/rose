add_library(roseBinaryAnalysisVariables OBJECT
  BaseVariable.C
  BasicTypes.C
  GlobalVariable.C
  InstructionAccess.C
  StackFrame.C
  StackVariable.C
  VariableFinder.C
)

add_dependencies(roseBinaryAnalysisVariables rosetta_generated)

install(FILES
  BaseVariable.h
  BasicTypes.h
  GlobalVariable.h
  InstructionAccess.h
  StackFrame.h
  StackVariable.h
  VariableFinder.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Variables)
