add_subdirectory(parser_support)


# GENERATING THE ROSE PREPROCESSOR
add_custom_command(
   OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/MatlabFrontend/lex.matlab.c
   COMMAND ${FLEX_EXECUTABLE} -t
   ${ROSE_TOP_SRC_DIR}/src/frontend/MatlabFrontend/matlab_lex.ll
   >  ${CMAKE_BINARY_DIR}/src/frontend/MatlabFrontend/lex.matlab.c

      DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/MatlabFrontend/matlab_lex.ll
)

add_custom_command(
   OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/matlab_parse.C
   COMMAND ${BISON_EXECUTABLE} -d
   ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/matlab_parse.yy
   ${CMAKE_BINARY_DIR}/src/frontend/SageIII/matlab_parse.C
   DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/matlab_parse.yy
)

add_custom_target( OMPPARSER ALL DEPENDS
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp-lex.yy.C
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C 
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.h
    COMMENT "Generating files for the omp parser" 
    )

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
  ${CMAKE_CURRENT_BINARY_DIR}/omp-lex.yy.C
)
add_dependencies(sage3 rosetta_generated)


########### install files ###############

install(
  FILES
    sage3.h sage3basic.h rose_attributes_list.h attachPreprocessingInfo.h
    attachPreprocessingInfoTraversal.h attach_all_info.h manglingSupport.h
    C++_include_files.h fixupCopy.h general_token_defs.h rtiHelpers.h
    ompAstConstruction.h  OmpAttribute.h omp.h dwarfSupport.h
    omp_lib_kinds.h omp_lib.h rosedll.h fileoffsetbits.h rosedefs.h
    sage3basic.hhh sage_support/cmdline.h sage_support/sage_support.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_Grammar.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarMemoryPoolSupport.h
    ${CMAKE_CURRENT_BINARY_DIR}/Cxx_GrammarTreeTraversalAccessEnums.h
    ${CMAKE_CURRENT_BINARY_DIR}/AST_FILE_IO.h
    ${CMAKE_CURRENT_BINARY_DIR}/StorageClasses.h
    ${CMAKE_CURRENT_BINARY_DIR}/AstQueryMemoryPool.h
    ${CMAKE_CURRENT_BINARY_DIR}/astFileIO/AstSpecificDataManagingClass.h
  DESTINATION ${INCLUDE_INSTALL_DIR} )
