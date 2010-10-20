
include_directories( ${ROSE_INCLUDES} )
#include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

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
add_custom_target( ROSETTA ALL 
       DEPENDS ${ROSETTA_HEADERS} ${ROSETTA_SRC} ${STORAGECLASSES_SRC}
    )

#add_custom_target( ROSETTA ALL DEPENDS copy_ROSETTA COMMENT "Generating files" )
#add_custom_target( ROSETTA ALL DEPENDS ${GENERATED_HEADERS} ${GENERATED_SRC} COMMENT "Generating files" )



########### install files ###############
#INSTALL(TARGET ${GENERATED_SRC} ${GENERATED_HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/src/frontend/SageIII/    )



#original Makefile.am contents follow:


#
#INCLUDES = $(ROSE_INCLUDES)
#
#EXTRA_DIST = \
#   ROSETTA_macros.h  grammar.h grammarString.h terminal.h
#
## Define the template repository path used in the autoconf
## subsitution for various C++ template control variables.
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#CXX_GRAMMAR_DIR   = $(top_builddir)/src/frontend/SageIII
#
#CXX_PREPROC_CODES = $(CXX_GRAMMAR_DIR)/Cxx_Grammar.h
#
##                   $(CXX_GRAMMAR_DIR)/Cxx_GrammarCheckingIfDataMembersAreInMemoryPool.C
#
#BUILT_SOURCES = $(CXX_PREPROC_CODES)
#
## The generation of the ROSE Grammars depend upon source code from these files
#GRAMMAR_SOURCE_FILES = \
#     ../Grammar/Statement.code \
#     ../Grammar/grammarClassDefinitionMacros.macro \
#     ../Grammar/grammarVariantFunctionDefinitionMacros.macro \
#     ../Grammar/Common.code \
#     ../Grammar/Support.code \
#     ../Grammar/grammarConstructorDefinitionMacros.macro \
#     ../Grammar/node.macro \
#     ../Grammar/Cxx_GlobalDeclarations.macro \
#     ../Grammar/Symbol.code \
#     ../Grammar/grammarDestructorDefinitionMacros.macro \
#     ../Grammar/parserCode.macro \
#     ../Grammar/Cxx_Support_Header.macro \
#     ../Grammar/Type.code \
#     ../Grammar/grammarIsClassNameFunctionDefinitionMacros.macro \
#     ../Grammar/parserPrototype.macro \
#     ../Grammar/Expression.code \
#     ../Grammar/copyMemberFunction.macro \
#     ../Grammar/grammarMainClassDeclatationMacros.macro \
#     ../Grammar/parserSourceCode.macro \
#     ../Grammar/LocatedNode.code \
#     ../Grammar/dataMemberAccessFunctions.macro \
#     ../Grammar/grammarMainClassParseFunctionSourceCode.macro \
#     ../Grammar/Node.code \
#     ../Grammar/BinaryInstruction.code \
#     ../Grammar/grammarClassDeclarationMacros.macro \
#     ../Grammar/grammarMainClassSourceCodeMacros.macro \
#     ../Grammar/grammarNewDeleteOperatorMacros.macro \
#     ../Grammar/grammarTraverseMemoryPool.macro \
#     ../Grammar/grammarCheckingIfDataMembersAreInMemoryPool.macro \
#     ../Grammar/grammarStorageClassDeclatationMacros.macro \
#     ../Grammar/grammarStorageClassDefinitionMacros.macro \
#     ../Grammar/grammarAST_FileIoHeader.code \
#     ../Grammar/grammarAST_FileIoSource.code  \
#     ../Grammar/grammarSourceOfIRNodesAstFileIOSupport.macro \
#     ../Grammar/grammarStaticDataManagingClassHeader.macro \
#     ../Grammar/grammarStaticDataManagingClassStorageClassHeader.macro \
#     ../Grammar/grammarReturnDataMemberPointers.macro \
#     ../Grammar/grammarProcessDataMemberReferenceToPointers.macro \
#     ../Grammar/grammarGetChildIndex.macro \
#     ../astNodeList
#
## DQ (4/6/2006): Removed from Jochen's new version
## ../Grammar/grammarConstructorOfStorageClass.macro
#
#
#
## VALGRIND_OPTIONS = -v --num-callers=12 --leak-check=yes --error-limit=no --show-reachable=yes
#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
## VALGRIND = valgrind $(VALGRIND_OPTIONS)
#VALGRIND =
#
## Build the grammar for generating the C++ preprocessor.
#$(CXX_PREPROC_CODES): CxxGrammarMetaProgram$(EXEEXT) $(GRAMMAR_SOURCE_FILES)
##  Build a link so that the executable can find the *.code file in ../Grammar
##	rm -f Grammar
##	ln -s $(top_srcdir)/src/ROSETTA/Grammar Grammar
#	$(VALGRIND) ./CxxGrammarMetaProgram
##	Files generated by ROSETTA must now be moved to $(CXX_GRAMMAR_DIR)
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_Grammar.h
#	mv Cxx_Grammar.h $(CXX_GRAMMAR_DIR)/Cxx_Grammar.h
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarMemoryPoolSupport.h
#	mv Cxx_GrammarMemoryPoolSupport.h $(CXX_GRAMMAR_DIR)/Cxx_GrammarMemoryPoolSupport.h
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarMemoryPoolSupport.C
#	mv Cxx_GrammarMemoryPoolSupport.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarMemoryPoolSupport.C
#	rm -f $(CXX_GRAMMAR_DIR)/AstQueryMemoryPool.h
#	mv AstQueryMemoryPool.h $(CXX_GRAMMAR_DIR)/AstQueryMemoryPool.h
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarReturnClassHierarchySubTree.C
#	mv Cxx_GrammarReturnClassHierarchySubTree.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarReturnClassHierarchySubTree.C
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_Grammar.C
#	mv Cxx_Grammar.C $(CXX_GRAMMAR_DIR)/Cxx_Grammar.C
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarTreeTraversal.C
#	mv Cxx_GrammarTreeTraversalSuccessorContainer.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarTreeTraversalSuccessorContainer.C
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarRTI.C
#	mv Cxx_GrammarRTI.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarRTI.C
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarTreeTraversalAccessEnums.h
#	mv Cxx_GrammarTreeTraversalAccessEnums.h $(CXX_GRAMMAR_DIR)/Cxx_GrammarTreeTraversalAccessEnums.h
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarVariantEnumNames.C
#	mv Cxx_GrammarVariantEnumNames.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarVariantEnumNames.C
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarNewAndDeleteOperators.C
#	mv Cxx_GrammarNewAndDeleteOperators.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarNewAndDeleteOperators.C
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarCopyMemberFunctions.C
#	mv Cxx_GrammarCopyMemberFunctions.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarCopyMemberFunctions.C
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarTraverseMemoryPool.C
#	mv Cxx_GrammarTraverseMemoryPool.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarTraverseMemoryPool.C
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarCheckingIfDataMembersAreInMemoryPool.C
#	mv Cxx_GrammarCheckingIfDataMembersAreInMemoryPool.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarCheckingIfDataMembersAreInMemoryPool.C
#	rm -f $(CXX_GRAMMAR_DIR)/astFileIO/AST_FILE_IO.h
##  DQ (10/18/2007): Increase available parallelism by putting large files together so that make -jn is more efficient.
##	mv AST_FILE_IO.h $(CXX_GRAMMAR_DIR)/astFileIO/AST_FILE_IO.h
#	mv AST_FILE_IO.h $(CXX_GRAMMAR_DIR)/AST_FILE_IO.h
#	rm -f $(CXX_GRAMMAR_DIR)/astFileIO/AST_FILE_IO.C
##  DQ (10/18/2007): Increase available parallelism by putting large files together so that make -jn is more efficient.
##	mv AST_FILE_IO.C $(CXX_GRAMMAR_DIR)/astFileIO/AST_FILE_IO.C
#	mv AST_FILE_IO.C $(CXX_GRAMMAR_DIR)/AST_FILE_IO.C
#	rm -f $(CXX_GRAMMAR_DIR)/astFileIO/StorageClasses.h
##  DQ (10/18/2007): Increase available parallelism by putting large files together so that make -jn is more efficient.
##	mv StorageClasses.h $(CXX_GRAMMAR_DIR)/astFileIO/StorageClasses.h
#	mv StorageClasses.h $(CXX_GRAMMAR_DIR)/StorageClasses.h
#	rm -f $(CXX_GRAMMAR_DIR)/astFileIO/StorageClasses.C
##  DQ (10/18/2007): Increase available parallelism by putting large files together so that make -jn is more efficient.
##	mv StorageClasses.C $(CXX_GRAMMAR_DIR)/astFileIO/StorageClasses.C
#	mv StorageClasses.C $(CXX_GRAMMAR_DIR)/StorageClasses.C
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarReturnDataMemberPointers.C
#	mv Cxx_GrammarReturnDataMemberPointers.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarReturnDataMemberPointers.C
##	DQ (4/30/2006): Added support for function to return references to IR nodes pointers
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarProcessDataMemberReferenceToPointers.C
#	mv Cxx_GrammarProcessDataMemberReferenceToPointers.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarProcessDataMemberReferenceToPointers.C
##	DQ (4/6/2006): Added from Jochen's multifile I/O support
#	rm -f $(CXX_GRAMMAR_DIR)/astFileIO/SourcesOfIRNodesAstFileIOSupport.C
#	mv SourcesOfIRNodesAstFileIOSupport.C $(CXX_GRAMMAR_DIR)/astFileIO/SourcesOfIRNodesAstFileIOSupport.C
#	rm -f $(CXX_GRAMMAR_DIR)/astFileIO/AstSpecificDataManagingClass.h
#	mv AstSpecificDataManagingClass.h $(CXX_GRAMMAR_DIR)/astFileIO/AstSpecificDataManagingClass.h
##	DQ (11/7/2006): Added support for new forms of constructors (without source position information)
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarNewConstructors.C
#	mv Cxx_GrammarNewConstructors.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarNewConstructors.C
##	DQ (3/8/2007): Added support for getChildIndex function
#	rm -f $(CXX_GRAMMAR_DIR)/Cxx_GrammarGetChildIndex.C
#	mv Cxx_GrammarGetChildIndex.C $(CXX_GRAMMAR_DIR)/Cxx_GrammarGetChildIndex.C
#
#noinst_PROGRAMS = CxxGrammarMetaProgram
#CxxGrammarMetaProgram_SOURCES = \
#   CxxGrammarMetaProgram.C \
#   grammar.C \
#   grammarString.C \
#   terminal.C \
#   type.C \
#   symbol.C \
#   expression.C \
#   statement.C \
#   support.C \
#   binaryInstruction.C \
#   storageSize.C \
#   visitorSupport.C \
#   poolBasedTraversal.C \
#   node.C \
#   buildAstFileIO.C \
#   buildStorageClasses.C \
#   buildConstructorsWithoutSourcePositionInformation.C \
#   outputClassesAndFields.C
#
## buildConstructorsWithoutSourcePositionInformation.o: buildConstructorsWithoutSourcePositionInformation.C
#
#CxxGrammarMetaProgram_LDADD = $(top_builddir)/src/util/libroseutil.la
#
## DQ (9/11/2008):
## The rose_paths.c is built in ROSE/src/util and this dependence causes
## ROSE to rebuild a lot of code each time configure is rerun. Let's see
## if this can be make a little more tolerable for those without larger
## computers working on ROSE (e.g. students).
## CxxGrammarMetaProgram_DEPENDENCIES = $(CxxGrammarMetaProgram_LDADD)
## CxxGrammarMetaProgram_DEPENDENCIES = 
## CxxGrammarMetaProgram_DEPENDENCIES = $(CxxGrammarMetaProgram_SOURCES)
#
#
## DQ (7/8/2005): Avoid building Sage III when util changes!
## $(top_builddir)/util/libroseutil.so:
##	cd $(top_builddir)/util; $(MAKE)
#
#
## Force recompilation of source files if any of the header files 
## are changed (does not work if *.o files are not present).
#$(librosetta_a_OBJECTS): ROSETTA_macros.h grammar.h grammarString.h terminal.h
#
#clean-local:
#	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files cxx_templates Cxx_Grammar* grammar.dot translationSupport.code
#
#distclean-local:
#	rm -rf Cxx_Grammar.h Cxx_Grammar.C Cxx_GrammarTreeTraversal.C Cxx_GrammarTreeTraversalClass.h \
#          Cxx_GrammarTreeTraversalClass.C
#	rm -rf grammar.tex grammar.dot
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
