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
  astSupport/StmtInfoCollect.C
  dependenceTracking/dependence_analysis.C
  dependenceTracking/dependence_table.C
  AstUtilInterface.C)
add_dependencies(midend_util rosetta_generated)
if(NOT WIN32)
  # AstUtilInterface breaks on Windows and is only installed for standalone tests on linux 
  install(FILES AstUtilInterface.h DESTINATION ${INCLUDE_INSTALL_DIR})
endif() 
add_subdirectory(astInterface)
add_subdirectory(astSupport)
add_subdirectory(symbolicVal)
add_subdirectory(annotation)
add_subdirectory(dependenceTracking)
