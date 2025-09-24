include_directories( ${ROSE_INCLUDES} )

########### next target ###############

if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR_DEFINED=\"${CMAKE_SOURCE_DIR}\"")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -DROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR_DEFINED=\"${CMAKE_SOURCE_DIR}\"")
endif()

# Rasmussen (04/17/2019): Support for ATerms has been deprecated as it is no longer needed
# and likely never fully implemented nor tested.  Files remain in the src tree but are no
# longer built. Files buildAtermSupport.C and grammarAtermConstructorDefinitionMacros.macro
# has been removed.

# source files for the code generator
set(CxxGrammarMetaProgram_SRCS
  CxxGrammarMetaProgram.C
  grammar.C
  GrammarFile.C
  grammarString.C
  grammarGenerator.C
  AstNodeClass.C
  type.C
  symbol.C
  expression.C
  statement.C
  support.C
  binaryInstruction.C
  storageSize.C
  visitorSupport.C
  poolBasedTraversal.C
  node.C
  nodeIdSupport.C
  buildAstFileIO.C
  buildRoseTraits.C
  buildStorageClasses.C
  buildConstructorsWithoutSourcePositionInformation.C
  outputClassesAndFields.C
  StringUtility.C
  jovialNodes.C)

# input files to generate source files
set(GRAMMAR_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/Node.code 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/Statement.code 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarClassDefinitionMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarVariantFunctionDefinitionMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/Common.code 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/Support.code 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarConstructorDefinitionMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/node.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/Cxx_GlobalDeclarations.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/Symbol.code 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarDestructorDefinitionMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/parserCode.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/Cxx_Support_Header.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/Type.code 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarIsClassNameFunctionDefinitionMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/parserPrototype.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/Expression.code 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/copyMemberFunction.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarMainClassDeclatationMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/parserSourceCode.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/LocatedNode.code 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/dataMemberAccessFunctions.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarMainClassParseFunctionSourceCode.macro
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarClassDeclarationMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarMainClassSourceCodeMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarNewDeleteOperatorMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarTraverseMemoryPool.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarNodeId.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarCheckingIfDataMembersAreInMemoryPool.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarStorageClassDefinitionMacros.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarAST_FileIoHeader.code 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarAST_FileIoSource.code  
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarSourceOfIRNodesAstFileIOSupport.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarStaticDataManagingClassHeader.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarStaticDataManagingClassStorageClassHeader.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarReturnDataMemberPointers.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarProcessDataMemberReferenceToPointers.macro 
  ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarGetChildIndex.macro 
  ../astNodeList)

add_executable(CxxGrammarMetaProgram ${CxxGrammarMetaProgram_SRCS})
add_dependencies(CxxGrammarMetaProgram roseUtil)

# Build ROSETTA code generator which is CxxGrammarMetaProgram
# Link to what librose.so links on Windows, Mac, or Linux using link_with_libraries
target_link_libraries(CxxGrammarMetaProgram ${link_with_libraries} roseUtil)

# define a custom command to call the code generator and generate source/header files
# Generate the edg-sage connection using ROSETTA
add_custom_command(
  OUTPUT ${ROSETTA_HEADERS} ${ROSETTA_SRC} 
  # depend on the code generator and input files
  DEPENDS CxxGrammarMetaProgram ${GRAMMAR_SOURCE_FILES} 
  # We need to copy the input files to the build tree since the code generator will output the generated source files
  # into the same directory as where the input files locate
  COMMAND ${CMAKE_COMMAND}  -E copy_directory  ${ROSE_TOP_SRC_DIR}/src/ROSETTA/Grammar ${ROSE_TOP_BINARY_DIR}/src/ROSETTA/Grammar
  # create this directory is needed since a few generated files will be put there        
  COMMAND ${CMAKE_COMMAND}  -E make_directory  ${ROSETTA_OUTPUT_DIRECTORY}/astFileIO
  COMMAND ${CMAKE_COMMAND}  -E make_directory  ${ROSETTA_OUTPUT_DIRECTORY}/Rose/Traits
  # create all class definition files whether they're needed or not since CxxGrammarMetaProgram won't create them all

  # Windows doesn't support very long commands, so we have to split these up
  #COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS}
  #COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_01}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_02}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_03}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_04}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_05}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_06}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_07}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_08}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_09}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_10}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_11}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_12}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_13}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_14}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_15}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_16}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_17}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_18}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_19}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_20}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_21}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_22}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_23}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_24}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_25}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_26}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_27}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_28}
  COMMAND ${CMAKE_COMMAND} -E rm -f ${ROSETTA_GENERATED_DEFINITIONS_29}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_01}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_02}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_03}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_04}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_05}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_06}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_07}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_08}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_09}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_10}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_11}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_12}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_13}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_14}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_15}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_16}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_17}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_18}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_19}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_20}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_21}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_22}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_23}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_24}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_25}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_26}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_27}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_28}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_29}
  COMMAND ${CMAKE_COMMAND} -E touch ${ROSETTA_GENERATED_DEFINITIONS_30}

  # COMMAND echo  "META_EXE_LOC =  ${META_EXE_LOC} ARGS ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/"
  # STRING(REGEX REPLACE "\\\\" "/" META_EXE_LOC ${META_EXE_LOC} )
  # COMMAND echo  "META_EXE_LOC =  ${META_EXE_LOC} ARGS ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/"
  COMMAND CxxGrammarMetaProgram
  # COMMAND "C:/ROSE/git-rose-tps/build/src/ROSETTA/src/Debug/CxxGrammarMetaProgram.exe "
  # The slash after the first argument is required.
  ARGS ${ROSETTA_OUTPUT_DIRECTORY}/)

# a custom target depending on generated rosetta source and header files to trigger the building process
add_custom_target(rosetta_generated ALL DEPENDS ${ROSETTA_HEADERS} ${ROSETTA_SRC} ${STORAGECLASSES_SRC})
