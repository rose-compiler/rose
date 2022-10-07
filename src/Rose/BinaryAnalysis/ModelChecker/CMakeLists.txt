add_library(roseBinaryAnalysisModelChecker OBJECT
  BasicBlockUnit.C
  Engine.C
  ErrorTag.C
  ExecutionUnit.C
  ExternalFunctionUnit.C
  FailureUnit.C
  InstructionUnit.C
  NullDereferenceTag.C
  OutOfBoundsTag.C
  PartitionerModel.C
  Path.C
  PathNode.C
  PathPredicate.C
  PathPrioritizer.C
  PathQueue.C
  Periodic.C
  Settings.C
  SemanticCallbacks.C
  SourceLister.C
  Tag.C
  UninitializedVariableTag.C
  WorkerStatus.C)

add_dependencies(roseBinaryAnalysisModelChecker rosetta_generated)

install(FILES
  BasicBlockUnit.h
  Engine.h
  ErrorTag.h
  Exception.h
  ExecutionUnit.h
  ExternalFunctionUnit.h
  FailureUnit.h
  InstructionUnit.h
  NullDereferenceTag.h
  OutOfBoundsTag.h
  PartitionerModel.h
  Path.h
  PathNode.h
  PathPredicate.h
  PathPrioritizer.h
  PathQueue.h
  Periodic.h
  Settings.h
  SemanticCallbacks.h
  SourceLister.h
  Tag.h
  Types.h
  UninitializedVariableTag.h
  WorkerStatus.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/ModelChecker)
