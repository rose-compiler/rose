add_subdirectory(CxxCodeGeneration)
add_subdirectory(languageIndependenceSupport)
add_subdirectory(formatSupport)

if (WIN32)
  message ("WARNING : Fortran and PHP not supported yet on Windows")
else (WIN32)
  # tps (11/23/2009) commented out fortran and php for windows for now
  if(enable-fortran)
    add_subdirectory(FortranCodeGeneration)
  endif()
  if(enable-java)
    add_subdirectory(JavaCodeGeneration)
  endif()
  if(enable-x10)
    add_subdirectory(X10CodeGeneration)
  endif()
  if(enable-php)
    add_subdirectory(PHPCodeGeneration)
  endif()
  if(enable-python)
    add_subdirectory(PythonCodeGeneration)
  endif()
endif (WIN32)


########### next target ###############

add_library (unparser OBJECT
  FortranCodeGeneration/unparseFortran_expressions.C
  FortranCodeGeneration/unparseFortran_modfile.C
  FortranCodeGeneration/unparseFortran_statements.C
  FortranCodeGeneration/unparseFortran_types.C
  JavaCodeGeneration/unparseJava_expressions.C
  JavaCodeGeneration/unparseJava_statements.C
  JavaCodeGeneration/unparseJava_types.C
  CxxCodeGeneration/unparseCxx_types.C
  CxxCodeGeneration/unparseCxx_expressions.C
  CxxCodeGeneration/unparseCxx_statements.C
  PHPCodeGeneration/unparsePHP.C
  astUnparseAttribute.C
  unparser.C
  nameQualificationSupport.C
  unparse_sym.C
  IncludedFilesUnparser.C
  formatSupport/unparseFormatHelp.C
  formatSupport/unparse_format.C
  languageIndependenceSupport/modified_sage_isUnaryOp.C
  languageIndependenceSupport/unparser_opt.C
  languageIndependenceSupport/modified_sage.C
  languageIndependenceSupport/name_qualification_support.C
  languageIndependenceSupport/unparse_debug.C
  languageIndependenceSupport/unparseLanguageIndependentConstructs.C
  X10CodeGeneration/unparseX10_expressions.C
  X10CodeGeneration/unparseX10_statements.C
  X10CodeGeneration/unparseX10_types.C)
add_dependencies(unparser rosetta_generated)

########### install files ###############

set(unparser_headers copy_unparser.h unparser.h unparse_sym.h
  astUnparseAttribute.h IncludedFilesUnparser.h nameQualificationSupport.h)

install(FILES ${unparser_headers} DESTINATION ${INCLUDE_INSTALL_DIR})
