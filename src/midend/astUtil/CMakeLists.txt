add_library(midend_util OBJECT
  astInterface/CPPAstInterface.C
  astInterface/AstInterface.C
  symbolicVal/SymbolicVal.C
  symbolicVal/SymbolicOperator.C
  symbolicVal/SymbolicUnwrapCond.C
  symbolicVal/SymbolicBound.C
  symbolicVal/SymbolicFind.C
  symbolicVal/SymbolicExpr.C
  annotation/OperatorAnnotation.C
  annotation/AnnotIO.C
  annotation/AnnotGen.C
  annotation/AnnotCollect.C
  annotation/AnnotDescriptors.C
  annotation/OperatorDescriptors.C
  annotation/AnnotExpr.C
  annotation/TypeAnnotation.C
  astSupport/ProcessAstTree.C
  astSupport/StmtInfoCollect.C)
add_dependencies(midend_util rosetta_generated)

add_subdirectory(astInterface)
add_subdirectory(astSupport)
add_subdirectory(symbolicVal)
add_subdirectory(annotation)
