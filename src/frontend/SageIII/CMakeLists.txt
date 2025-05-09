add_subdirectory(astFileIO)
add_subdirectory(astFixup)
add_subdirectory(astPostProcessing)
add_subdirectory(sageInterface)
add_subdirectory(virtualCFG)
add_subdirectory(astVisualization)

if(NOT ENABLE-INTERNALFRONTENDDEVELOPMENT)
  ##AS Commented out for simpler build
  ###add_subdirectory(astFileIO)
  add_subdirectory(astFromString)
  add_subdirectory(astHiddenTypeAndDeclarationLists)
  add_subdirectory(astTokenStream)
  add_subdirectory(includeDirectivesProcessing)
endif()

set(SAGE3_SOURCES
  rose_attributes_list.C
  attachPreprocessingInfo.C
  attachPreprocessingInfoTraversal.C
  attributeListMap.C
  manglingSupport.C
  sage_support/sage_support.C
  sage_support/cmdline.C
  sage_support/keep_going.C
  fixupCopy_scopes.C
  fixupCopy_symbols.C
  fixupCopy_references.C
  rtiHelpers.C
  dwarfSupport.C
  rose_graph_support.C
  Utf8.C)

########################################################################################################################
if(ENABLE-CPP)
  # GENERATING THE ROSE PREPROCESSOR

  if(FLEX_FOUND)
    add_custom_command(
      OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.C
      COMMAND ${FLEX_EXECUTABLE} -t ${CMAKE_CURRENT_SOURCE_DIR}/preproc-c.ll > ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.C
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/preproc-c.ll)

    add_custom_target(ROSE_PREPROCESSOR ALL DEPENDS
      ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.C COMMENT "Generating files for preprocessor" )

    # GENERATING the OpenMP parser
    add_custom_command(
      OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/omp-lex.yy.C
      COMMAND ${FLEX_EXECUTABLE} -t ${CMAKE_CURRENT_SOURCE_DIR}/omplexer.ll > ${CMAKE_CURRENT_BINARY_DIR}/omp-lex.yy.C
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/omplexer.ll)
  else()
    #Flex is not avaiable so use the pre generated files
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.C.pre
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.C.pre ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.C)

    add_custom_target(ROSE_PREPROCESSOR ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.C)

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/omp-lex.yy.C.pre
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/omp-lex.yy.C.pre ${CMAKE_CURRENT_BINARY_DIR}/omp-lex.yy.C)
  endif()

  if(BISON_FOUND)
    add_custom_command(
      OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/ompparser.C ${CMAKE_CURRENT_BINARY_DIR}/ompparser.h
      COMMAND ${BISON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/ompparser.yy -o ${CMAKE_CURRENT_BINARY_DIR}/ompparser.c
      COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/ompparser.c ${CMAKE_CURRENT_BINARY_DIR}/ompparser.C
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ompparser.yy)
  else() 
    #Bison unavaialbe so use pre generated files
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ompparser.C.pre ${CMAKE_CURRENT_SOURCE_DIR}/ompparser.h.pre
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/ompparser.C.pre ${CMAKE_CURRENT_BINARY_DIR}/ompparser.C)
    file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/ompparser.h.pre ${CMAKE_CURRENT_BINARY_DIR}/ompparser.h)
  endif()

  add_custom_target(OMPPARSER ALL DEPENDS
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp-lex.yy.C
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C
    COMMENT "Generating files for the omp parser")

  set(SAGE3_SOURCES ${SAGE3_SOURCES}
    OmpAttribute.C ompAstConstruction.cpp ompFortranParser.C
    ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.C
    ${CMAKE_CURRENT_BINARY_DIR}/ompparser.C
    ${CMAKE_CURRENT_BINARY_DIR}/omp-lex.yy.C)

endif()

########### next target ###############
add_library(sage3 OBJECT ${SAGE3_SOURCES})
add_dependencies(sage3 rosetta_generated)

# Add include directory to SageTreeBuilder files
if(ENABLE-JOVIAL OR ENABLE-ADA) 
  target_include_directories(sage3 PRIVATE ../Experimental_General_Language_Support)
endif()

########### install files ###############

install(
  FILES
    sage3.h sage3basic.h rose_attributes_list.h attachPreprocessingInfo.h
    attachPreprocessingInfoTraversal.h attach_all_info.h manglingSupport.h
    C++_include_files.h fixupCopy.h general_token_defs.h rtiHelpers.h
    ompAstConstruction.h  OmpAttribute.h omp.h
    omp_lib_kinds.h omp_lib.h rosedll.h fileoffsetbits.h rosedefs.h
    sage3basic.hhh sage_support/cmdline.h sage_support/sage_support.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarSerialization.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_Grammar.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarDeclarations.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarDowncast.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarStorageClasses.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarSgNodeDefinitions.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarSgAsmNodeDefinitions.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarSgAsmCilNodeDefinitions.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarSgAsmDwarfNodeDefinitions.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarSgAsmJvmNodeDefinitions.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarVariants.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarVisitorSupport.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarMemoryPoolSupport.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarTreeTraversalAccessEnums.h
    ${CMAKE_CURRENT_BINARY_DIR}/AST_FILE_IO.h
    ${CMAKE_CURRENT_BINARY_DIR}/StorageClasses.h
    ${CMAKE_CURRENT_BINARY_DIR}/AstQueryMemoryPool.h
    ${CMAKE_CURRENT_BINARY_DIR}/astFileIO/AstSpecificDataManagingClass.h
    ${ROSETTA_GENERATED_DEFINITIONS}
  DESTINATION ${INCLUDE_INSTALL_DIR})

install(FILES ${CMAKE_BINARY_DIR}/src/frontend/SageIII/Rose/Traits/generated.h DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Traits) 
