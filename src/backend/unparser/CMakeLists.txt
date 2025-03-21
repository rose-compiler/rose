add_subdirectory(CxxCodeGeneration)
add_subdirectory(languageIndependenceSupport)
add_subdirectory(formatSupport)

if(WIN32)
  message("WARNING : Fortran and PHP not supported yet on Windows")
else()
  # tps (11/23/2009) commented out fortran and php for windows for now

  # Excluding these subdirectories based on which analyses languages are supported is not sufficient because
  # the headers are still included from unparser.h and so need to still be installed.
  add_subdirectory(FortranCodeGeneration)
  add_subdirectory(JavaCodeGeneration)
  add_subdirectory(AdaCodeGeneration)
  add_subdirectory(JovialCodeGeneration)
  add_subdirectory(PHPCodeGeneration)
  add_subdirectory(PythonCodeGeneration)
endif()

########### next target ###############

add_library (unparser OBJECT
  FortranCodeGeneration/unparseFortran_expressions.C
  FortranCodeGeneration/unparseFortran_modfile.C
  FortranCodeGeneration/unparseFortran_statements.C
  FortranCodeGeneration/unparseFortran_types.C
  MatlabCodeGeneration/MatlabUnparserBottomUpTraversal.C
  MatlabCodeGeneration/MatlabUnparser.C
  JavaCodeGeneration/unparseJava_expressions.C
  JavaCodeGeneration/unparseJava_statements.C
  JavaCodeGeneration/unparseJava_types.C
  CxxCodeGeneration/unparseCxx_types.C
  CxxCodeGeneration/unparseCxx_expressions.C
  CxxCodeGeneration/unparseCxx_statements.C
  CxxCodeGeneration/unparseNewOp.C
  CxxCodeGeneration/unparseAggrInit.C
  CxxCodeGeneration/unparseAssnInit.C
  CxxCodeGeneration/unparseBracedInit.C
  CxxCodeGeneration/unparseCompInit.C
  CxxCodeGeneration/unparseClassType.C
  CxxCodeGeneration/unparseCtorInit.C
  CxxCodeGeneration/unparseDesignatedInitializer.C
  CxxCodeGeneration/unparseTemplateArgumentList.C
  CxxCodeGeneration/unparseType.C
  CxxCodeGeneration/unparseVarDeclStmt.C
  CxxCodeGeneration/helpers_types.C
  CxxCodeGeneration/helpers_stmts.C
  CxxCodeGeneration/helpers_exprs.C
  PHPCodeGeneration/unparsePHP.C
  astUnparseAttribute.C
  unparser.C
  nameQualificationSupport.C
  unparse_sym.C
  IncludedFilesUnparser.C
  includeFileSupport.C
  formatSupport/unparseFormatHelp.C
  formatSupport/unparse_format.C
  languageIndependenceSupport/modified_sage_isUnaryOp.C
  languageIndependenceSupport/unparser_opt.C
  languageIndependenceSupport/modified_sage.C
  languageIndependenceSupport/name_qualification_support.C
  languageIndependenceSupport/unparse_debug.C
  languageIndependenceSupport/unparseLanguageIndependentConstructs.C
  AdaCodeGeneration/unparseAda_expressions.C
  AdaCodeGeneration/unparseAda_statements.C
  AdaCodeGeneration/unparseAda_types.C
  JovialCodeGeneration/unparseJovial_expressions.C
  JovialCodeGeneration/unparseJovial_modfile.C
  JovialCodeGeneration/unparseJovial_statements.C
  JovialCodeGeneration/unparseJovial_types.C)
add_dependencies(unparser rosetta_generated)

########### install files ###############

set(unparser_headers

  astUnparseAttribute.h
  copy_unparser.h
  IncludedFilesUnparser.h
  includeFileSupport.h
  nameQualificationSupport.h
  UnparserDelegate.h
  unparser.h
  unparse_sym.h
)

install(FILES ${unparser_headers} DESTINATION ${INCLUDE_INSTALL_DIR})
