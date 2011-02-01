
include_directories( ${ROSE_INCLUDES} )

########### next target ###############

if (WIN32)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR_DEFINED=\"${CMAKE_SOURCE_DIR}\"" )
else (WIN32)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -DROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR_DEFINED=\"${CMAKE_SOURCE_DIR}\"" )
endif (WIN32)

# source files for the code generator
set(CxxGrammarMetaProgram_SRCS
    CxxGrammarMetaProgram.C
    grammar.C
    grammarString.C
    terminal.C
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
    buildAstFileIO.C
    buildStorageClasses.C
    buildConstructorsWithoutSourcePositionInformation.C
    outputClassesAndFields.C)

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
     ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/BinaryInstruction.code 
     ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarClassDeclarationMacros.macro 
     ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarMainClassSourceCodeMacros.macro 
     ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarNewDeleteOperatorMacros.macro 
     ${CMAKE_SOURCE_DIR}/src/ROSETTA/Grammar/grammarTraverseMemoryPool.macro 
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
     ../astNodeList
   )

add_executable(CxxGrammarMetaProgram ${CxxGrammarMetaProgram_SRCS} ${rose_util_src})

# build ROSETTA code generator
#target_link_libraries(CxxGrammarMetaProgram ${KDE4_KDECORE_LIBS} rose_util_lib ${Boost_LIBRARIES})
if (NOT WIN32)
  target_link_libraries(CxxGrammarMetaProgram ${KDE4_KDECORE_LIBS}  ${Boost_LIBRARIES})
else (NOT WIN32)
  target_link_libraries(CxxGrammarMetaProgram ${KDE4_KDECORE_LIBS}  ${Boost_LIBRARIES} shlwapi.lib)
endif (NOT WIN32)



get_target_property (META_EXE_LOC CxxGrammarMetaProgram LOCATION) 
message(STATUS "META_EXE_LOC = " ${META_EXE_LOC} )
#STRING(REGEX REPLACE "Debug" " " META_EXE_LOC2 ${META_EXE_LOC} )
#message(STATUS "META_EXE_LOC2 = " ${META_EXE_LOC2} )


if ( enable-smaller-generated-files  )
message (STATUS "Smaller Generated Files Enabled")
# define a custom command to call the code generator and generate source/header files
# Generate the edg-sage connection using ROSETTA
add_custom_command(
   OUTPUT ${ROSETTA_HEADERS} ${ROSETTA_SRC} 
#  depend on the code generator and input files
   DEPENDS CxxGrammarMetaProgram ${GRAMMAR_SOURCE_FILES} libgeneratedIR   
#  We need to copy the input files  to the build tree since 
#  the code generator will output the generated source files into the same directory as where the input files locate
   COMMAND
       ${CMAKE_COMMAND}  -E copy_directory  ${ROSE_TOP_SRC_DIR}/src/ROSETTA/Grammar ${ROSE_TOP_BINARY_DIR}/src/ROSETTA/Grammar
#   create this directory is needed since a few generated files will be put there        
   COMMAND
      ${CMAKE_COMMAND}  -E make_directory  ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/astFileIO
   COMMAND ${META_EXE_LOC}
      ARGS ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/
)

else ( enable-smaller-generated-files  )
message (STATUS "Smaller Generated Files Disabled")

# define a custom command to call the code generator and generate source/header files
# Generate the edg-sage connection using ROSETTA
add_custom_command(
   OUTPUT ${ROSETTA_HEADERS} ${ROSETTA_SRC} 
#  depend on the code generator and input files
   DEPENDS CxxGrammarMetaProgram ${GRAMMAR_SOURCE_FILES} 
#  We need to copy the input files  to the build tree since 
#  the code generator will output the generated source files into the same directory as where the input files locate
   COMMAND
       ${CMAKE_COMMAND}  -E copy_directory  ${ROSE_TOP_SRC_DIR}/src/ROSETTA/Grammar ${ROSE_TOP_BINARY_DIR}/src/ROSETTA/Grammar
#   create this directory is needed since a few generated files will be put there        
   COMMAND
      ${CMAKE_COMMAND}  -E make_directory  ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/astFileIO
   COMMAND echo  "META_EXE_LOC =  ${META_EXE_LOC} ARGS ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/"
#   STRING(REGEX REPLACE "\\\\" "/" META_EXE_LOC ${META_EXE_LOC} )
#   COMMAND echo  "META_EXE_LOC =  ${META_EXE_LOC} ARGS ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/"
   COMMAND ${META_EXE_LOC}
#   COMMAND "C:/ROSE/git-rose-tps/build/src/ROSETTA/src/Debug/CxxGrammarMetaProgram.exe "
      ARGS ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/
)


endif ( enable-smaller-generated-files  )



# a custom target depending on generated rosetta source and header files to trigger the building process
add_custom_target( rosetta_generated ALL 
       DEPENDS ${ROSETTA_HEADERS} ${ROSETTA_SRC} ${STORAGECLASSES_SRC}
    )




########### install files ###############
#INSTALL(TARGET ${GENERATED_SRC} ${GENERATED_HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/src/frontend/SageIII/    )


