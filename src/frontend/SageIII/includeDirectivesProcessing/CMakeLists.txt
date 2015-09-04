add_library( includeDirectivesProcessing OBJECT
  CompilerOutputReader.C
  CompilerOutputParser.C
  IncludingPreprocessingInfosCollector.C)
add_dependencies(includeDirectivesProcessing rosetta_generated)


########### install files ###############

install(FILES
  CompilerOutputReader.h CompilerOutputParser.h
  IncludingPreprocessingInfosCollector.h
  DESTINATION ${INCLUDE_INSTALL_DIR})

