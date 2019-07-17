add_subdirectory(astFileIO)
# tps (04/01/2010) : Added  GENERATED_CODE_DIRECTORY_Cxx_Grammar subdirectory
add_subdirectory(GENERATED_CODE_DIRECTORY_Cxx_Grammar)
add_subdirectory(astFixup)
add_subdirectory(astPostProcessing)
add_subdirectory(sageInterface)
add_subdirectory(virtualCFG)
add_subdirectory(astVisualization)

if(NOT enable-internalFrontendDevelopment)
  ##AS Commented out for simpler build
  ###add_subdirectory(astFileIO)
  add_subdirectory(astFromString)
  add_subdirectory(astHiddenTypeAndDeclarationLists)
  add_subdirectory(astMerge)
  add_subdirectory(astTokenStream)
  add_subdirectory(includeDirectivesProcessing)
endif()

########### next target ###############

# GENERATING THE ROSE PREPROCESSOR
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/lex.yy.C
  COMMAND ${FLEX_EXECUTABLE} -t
    ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/preproc-c.ll > ${CMAKE_BINARY_DIR}/src/frontend/SageIII/lex.yy.C
  DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/preproc-c.ll)

add_custom_target( ROSE_PREPROCESSOR ALL DEPENDS
  ${CMAKE_BINARY_DIR}/src/frontend/SageIII/lex.yy.C COMMENT "Generating files for preprocessor" )

########### next target ###############

#GENERATING the OpenMP parser
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp-lex.yy.C
  COMMAND ${FLEX_EXECUTABLE} -t
    ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/omplexer.ll > ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp-lex.yy.C
  DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/omplexer.ll)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C
  COMMAND ${BISON_EXECUTABLE}
    ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/ompparser.yy -o
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C
  DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/ompparser.yy)

#BISON generates a ompparser.H file and we need a ompparser.h file
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.h
  COMMAND ${CMAKE_COMMAND} -E rename
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.H ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.h
  DEPENDS ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C)

add_custom_target(OMPPARSER ALL DEPENDS
  ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp-lex.yy.C
  ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C 
  ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.h
  COMMENT "Generating files for the omp parser")

#Mark these files as generated
#SET_SOURCE_FILES_PROPERTIES(
#    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/lex.yy.C
#    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C
#    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.h
#    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp-lex.yy.C
#    PROPERTIES GENERATED 1
#    )


########### next target ###############
add_library(sage3 OBJECT
  rose_attributes_list.C
  attachPreprocessingInfo.C
  attachPreprocessingInfoTraversal.C
  attributeListMap.C
  manglingSupport.C
  sage_support/sage_support.cpp
  sage_support/cmdline.cpp
  sage_support/keep_going.cpp
  fixupCopy_scopes.C
  fixupCopy_symbols.C
  fixupCopy_references.C
  rtiHelpers.C
  OmpAttribute.C
  ompAstConstruction.cpp
  ompFortranParser.C
  dwarfSupport.C
  rose_graph_support.C
  #omplexer.ll
  #ompparser.yy
  Utf8.C
  ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.C
  ${CMAKE_CURRENT_BINARY_DIR}/ompparser.C
  ${CMAKE_CURRENT_BINARY_DIR}/omp-lex.yy.C)
add_dependencies(sage3 rosetta_generated)


########### install files ###############

# Rasmussen (04/17/2019): Support for ATerms has been deprecated (atermSupport.h removed)

install(
  FILES
    sage3.h sage3basic.h rose_attributes_list.h attachPreprocessingInfo.h
    attachPreprocessingInfoTraversal.h attach_all_info.h manglingSupport.h
    C++_include_files.h fixupCopy.h general_token_defs.h rtiHelpers.h
    ompAstConstruction.h  OmpAttribute.h omp.h dwarfSupport.h
    omp_lib_kinds.h omp_lib.h rosedll.h fileoffsetbits.h rosedefs.h
    sage3basic.hhh sage_support/cmdline.h sage_support/sage_support.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarSerialization.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_Grammar.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarMemoryPoolSupport.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarTreeTraversalAccessEnums.h
    ${CMAKE_CURRENT_BINARY_DIR}/AST_FILE_IO.h
    ${CMAKE_CURRENT_BINARY_DIR}/StorageClasses.h
    ${CMAKE_CURRENT_BINARY_DIR}/AstQueryMemoryPool.h
    ${CMAKE_CURRENT_BINARY_DIR}/astFileIO/AstSpecificDataManagingClass.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
