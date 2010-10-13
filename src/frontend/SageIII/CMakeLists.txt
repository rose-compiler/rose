
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )
##AS Commented out for simpler build
###add_subdirectory(astFileIO)
# tps (04/01/2010) : Added  GENERATED_CODE_DIRECTORY_Cxx_Grammar subdirectory
add_subdirectory(GENERATED_CODE_DIRECTORY_Cxx_Grammar)
add_subdirectory(astMerge)
add_subdirectory(astFixup)
add_subdirectory(astPostProcessing)
add_subdirectory(sageInterface)
add_subdirectory(virtualCFG)
add_subdirectory(astTokenStream)
add_subdirectory(astHiddenTypeAndDeclarationLists)
add_subdirectory(astVisualization)


########### next target ###############

# GENERATING THE ROSE PREPROCESSOR
add_custom_command(
   OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/lex.yy.C
   COMMAND ${FLEX_EXECUTABLE} -t
   ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/preproc-c.ll
   >  ${CMAKE_BINARY_DIR}/src/frontend/SageIII/lex.yy.C

      DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/preproc-c.ll
)

add_custom_target( ROSE_PREPROCESSOR ALL DEPENDS
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/lex.yy.C COMMENT "Generating files for preprocessor" )

########### next target ###############

#GENERATING the OpenMP parser
add_custom_command(
   OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp-lex.yy.C
   COMMAND ${FLEX_EXECUTABLE} -t
   ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/omplexer.ll
   >  ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp-lex.yy.C

   DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/omplexer.ll
)

add_custom_command(
   OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C
   COMMAND ${BISON_EXECUTABLE}
   ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/ompparser.yy -o
   ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C
   DEPENDS  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/ompparser.yy
)

#BISON generates a ompparser.H file and we need a ompparser.h file
add_custom_command(
   OUTPUT ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.h
   COMMAND 
   ${CMAKE_COMMAND}  -E copy
   ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.H ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.h
   DEPENDS ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C
  )

add_custom_target( OMPPARSER ALL DEPENDS
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/omp-lex.yy.C
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.C 
    ${CMAKE_BINARY_DIR}/src/frontend/SageIII/ompparser.h
    COMMENT "Generating files for the omp parser" 
    )


########### install files ###############

install(FILES  sage3.h sage3basic.h rose_attributes_list.h attachPreprocessingInfo.h     attachPreprocessingInfoTraversal.h attach_all_info.h manglingSupport.h C++_include_files.h     fixupCopy.h general_token_defs.h rtiHelpers.h   ompAstConstruction.h  OmpAttribute.h omp.h dwarfSupport.h     omp_lib_kinds.h omp_lib.h DESTINATION ${INCLUDE_INSTALL_DIR})
install(FILES  Cxx_Grammar.h  rosedll.h   Cxx_GrammarMemoryPoolSupport.h     Cxx_GrammarTreeTraversalAccessEnums.h     AST_FILE_IO.h StorageClasses.h     AstQueryMemoryPool.h     astFileIO/AstSpecificDataManagingClass.h DESTINATION ${INCLUDE_INSTALL_DIR}  )


#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## Include the "." to force the current directory to be compiled 
## before the subdirectories.
#SUBDIRS = astFileIO astMerge astFixup astPostProcessing sageInterface virtualCFG astTokenStream astHiddenTypeAndDeclarationLists astVisualization
#
## DQ (5/2/2009): This supports the optimization of the new 
## Graph IR nodes (makes about 25% improvement, I think).
## ROSE_OPTIMIZATION_LEVEL = -O3
#ROSE_OPTIMIZATION_LEVEL =
#
## Liao, 3/31/2009, moved the OpenMP parser here
##Liao, 10/27/2008. I decide to parse OpenMP pragma string here
## Bison flags
#AM_YFLAGS = -d
## Flex flags
#AM_LFLAGS =
## avoid unused Flex functions which are automatically generated
#CXXFLAGS+= -DYY_NO_TOP_STATE -DYY_NO_POP_STATE $(ROSE_OPTIMIZATION_LEVEL)
## omplexer.cc needs a header generaetd from ompparser.yy
#omplexer.lo: ompparser.h omplexer.cc
#ompparser.h: ompparser.cc
#
## Force sage.docs to be make by default 
## (if sage.docs.in is modified)
#all-local: roseh sage.docs 
#
#
## INCLUDES = $(ROSE_INCLUDES)
## INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src $(WAVE_INCLUDES)
## INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src
## JJW 6-2-2008: We shouldn't be using anything from EDG outside these two directories
#INCLUDES = $(ROSE_INCLUDES) # -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
## Copy the original lex file and the C++ file generated from it to the distribution
## so that users don't have to find an appropriate version of lex (flex).  Flex should
## be used by developers so that ANSI C code is generated for g++.  This is something
## for developers to worry about in the generation of sufficently portable distributions.
## EXTRA_DIST = preproc.lex preproc.C sageClasses.docs sage.docs.in docs
## EXTRA_DIST = preproc.lex preproc.C astGraphTemplate.C docs
## EXTRA_DIST = preproc.ll docs rtiHelpers.h attributeListMap.h
#
## DQ (9/29/2009): Renamed preproc.ll to preproc-c.ll in SVN.
## This will hopefull avoid a linking problem where multiple files
## are called preproc.o (from here and from EDG).
#EXTRA_DIST = preproc-c.ll docs rtiHelpers.h attributeListMap.h \
#             advanced_preprocessing_hooks.h rose_attributes_list.h dwarfSupport.h \
#             omplexer.cc ompparser.cc ompparser.h
#CLEANFILES = haveRosettaGeneratedSource
#
## DISTCLEANFILES = preproc.cc ompparser.cc ompparser.h omplexer.cc
#DISTCLEANFILES = preproc-c.cc ompparser.cc ompparser.h omplexer.cc
#
#
## DQ (10/18/2007): Added AST_FILE_IO.C StorageClasses.C
## Some source code is generated (call make in ROSE/src/ROSETTA/src to generate these files)
#GENERATED_SOURCE = \
#	Cxx_Grammar.h \
#	Cxx_GrammarRTI.C \
#	Cxx_Grammar.C \
#	Cxx_GrammarMemoryPoolSupport.C \
#	AstQueryMemoryPool.h \
#	Cxx_GrammarReturnClassHierarchySubTree.C \
#	Cxx_GrammarTreeTraversalSuccessorContainer.C \
#	Cxx_GrammarTreeTraversalAccessEnums.h \
#	Cxx_GrammarVariantEnumNames.C \
#	Cxx_GrammarNewAndDeleteOperators.C \
#	Cxx_GrammarCopyMemberFunctions.C \
#	Cxx_GrammarTraverseMemoryPool.C \
#	Cxx_GrammarCheckingIfDataMembersAreInMemoryPool.C \
#	Cxx_GrammarMemoryPoolSupport.h \
#	AST_FILE_IO.h \
#	AST_FILE_IO.C \
#	StorageClasses.h \
#	StorageClasses.C \
#	Cxx_GrammarReturnDataMemberPointers.C \
#	Cxx_GrammarProcessDataMemberReferenceToPointers.C \
#	Cxx_GrammarNewConstructors.C \
#	Cxx_GrammarGetChildIndex.C
#
#
## DQ (4/23/2009): This file depends upon other files
#StorageClasses.lo: $(srcdir)/astFileIO/StorageClassMemoryManagement.C astFileIO/SourcesOfIRNodesAstFileIOSupport.C
#
#haveRosettaGeneratedSource:
#	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)
#	touch haveRosettaGeneratedSource
#
#force-haveRosettaGeneratedSource:
#	-rm Cxx_Grammar.h
#	$(MAKE) haveRosettaGeneratedSource
#
#$(GENERATED_SOURCE): haveRosettaGeneratedSource
#
#libsage3Sources = \
#   rose_attributes_list.C \
#   attachPreprocessingInfo.C \
#   attachPreprocessingInfoTraversal.C \
#   attributeListMap.C \
#   manglingSupport.C \
#   sageSupport.C \
#   fixupCopy_scopes.C \
#   fixupCopy_symbols.C \
#   fixupCopy_references.C \
#   rtiHelpers.C \
#   OmpAttribute.C \
#   ompFortranParser.C \
#   dwarfSupport.C \
#   rose_graph_support.C
#
## $(GENERATED_SOURCE) is handled separately
#
## DQ (5/27/2007): Files moved or removed
##   generateAST.C
##   AstAttributeDOT.C
##   astGraph.C
##   insert.C
##   stripWrapper.C
##   grammarBaseClass.C
#
## DQ (12/16/2008): Removed as part of rewrite of CPP and comment handling.
##   attach_all_info.C 
#
#noinst_LTLIBRARIES = libsage3.la
#
## libsage3_la_SOURCES = $(libsage3Sources) preproc.ll rose_paths.h omplexer.ll ompparser.yy
## libsage3_la_SOURCES = $(libsage3Sources) preproc.ll omplexer.ll ompparser.yy
#libsage3_la_SOURCES = $(libsage3Sources) preproc-c.ll omplexer.ll ompparser.yy
#
## DQ (3/8/2009): I think that the dependence upon $(top_builddir)/rose_config.h 
## is what was forcing a lot of files to be compiled after reruning configure.
## nodist_libsage3_la_SOURCES = $(GENERATED_SOURCE) $(top_builddir)/rose_config.h
#nodist_libsage3_la_SOURCES = $(GENERATED_SOURCE)
#
#
#if ROSE_USE_BOOST_WAVE
#libsage3_la_LIBADD  = \
#	astFixup/libastFixup.la \
#	astPostProcessing/libastPostProcessing.la \
#	astMerge/libastMerge.la \
#	sageInterface/libsageInterface.la \
#	astTokenStream/libastTokenStream.la \
#	virtualCFG/libvirtualCFG.la \
#	astHiddenTypeAndDeclarationLists/libastHiddenTypeAndDeclarationLists.la \
#   astVisualization/libastVisualization.la
#else
#libsage3_la_LIBADD  = \
#	astFixup/libastFixup.la \
#	astPostProcessing/libastPostProcessing.la \
#	astMerge/libastMerge.la \
#	sageInterface/libsageInterface.la \
#	virtualCFG/libvirtualCFG.la \
#	astHiddenTypeAndDeclarationLists/libastHiddenTypeAndDeclarationLists.la \
#   astVisualization/libastVisualization.la
#endif
#
##	astFileIO/libastFileIO.la 
##	astFileIO/libastFileIO.la 
#
## $(CXX_TEMPLATE_OBJECTS)
## libsage3_la_DEPENDENCIES = preproc.o $(GENERATED_OBJECTS) astFixup/*.o astPostProcessing/*.o astMerge/*.o astFileIO/*.o sageInterface/*.o virtualCFG/*.o 
#
#rose_attributes_list.lo : rose_attributes_list.h 
#
#if ROSE_PCH
#if ROSE_MPI
## TPS (11Feb2009): PCH only works when multithreading is off.
## We should not use PCH and MPI together.
#roseh:
#
#else
## DQ (2/11/2009): This is part of the support for Precompiled Headers (PCH) using GNU g++
#roseh:
##	DQ (2/10/2009): Rush for the off switch on precompiled header files, comment out to skip use of PCH.
##       TPS (23 Feb 2009): Hardcoded -fpic and -DPIC for now because 
##       for some reason it is used by most files although not specified on configure
#	@echo "  COMPILE rose.h.gch"
#	@$(CXX)  -fPIC -DPIC $(CXXFLAGS) $(CFLAGS) $(CPPFLAGS) -g -o rose.h.gch $(top_srcdir)/src/rose.h -I$(top_builddir) $(ROSE_INCLUDES) 
#endif
#else
## default, do nothing.
#roseh:
#
#endif
#
#test_roseh: roseh
#
## DQ (10/18/2007): Added AST_FILE_IO.h StorageClasses.h
## Removed sla.h (redundant with location in src/util/commandlineProcessing/sla.h)
#include_HEADERS = \
#   sage3.h rose_attributes_list.h \
#   attachPreprocessingInfo.h \
#   attachPreprocessingInfoTraversal.h \
#   attach_all_info.h manglingSupport.h C++_include_files.h \
#   fixupCopy.h \
#   general_token_defs.h rtiHelpers.h \
#   OmpAttribute.h omp.h dwarfSupport.h \
#   omp_lib_kinds.h omp_lib.h
#
## DQ (4/5/2009): Moved rose_paths.h to src/util (where the source file is located).
#nodist_include_HEADERS = \
#   Cxx_Grammar.h \
#   Cxx_GrammarMemoryPoolSupport.h \
#   Cxx_GrammarTreeTraversalAccessEnums.h \
#   AST_FILE_IO.h StorageClasses.h \
#   AstQueryMemoryPool.h \
#   astFileIO/AstSpecificDataManagingClass.h
#
#BUILT_SOURCES = $(GENERATED_SOURCE) ompparser.cc ompparser.h omplexer.cc
#
## DQ (9/9/2007): Put C++_include_files.h back into place
## since it is a convinient mechanism to handle included headers 
## in the Cxx_Grammar.C file (a ROSETTA generated file).
## DQ (5/27/2007): Files moved or removed
## grammarBaseClass.h 
## AstAttributeDOT.h
## astGraph.h
## astGraphTemplate.C
## sageSupport.h
## C++_include_files.h
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#	rm -rf Cxx_Grammar* AST_FILE_IO.C StorageClasses.C
#	rm -rf AST_FILE_IO.h StorageClasses.h AstQueryMemoryPool.h
#	rm -rf generated_abstractcppgrammar.atg
#	rm -f rose.h.gch
#
#distclean-local:
#	rm -rf Templates.DB
#	rm -f rose.h.gch
#
#removeHashLineDirectivesFromGeneratedCode:
#	cp Cxx_Grammar.h  Cxx_Grammar.h.withLineDirectives
#	grep -v "#line" Cxx_Grammar.h > Cxx_Grammar.h.withoutLineDirectives
#	cp Cxx_Grammar.h.withoutLineDirectives Cxx_Grammar.h
#
#
#testRose:
#	/usr/bin/time roseAnalysis -rose:verbose 2 -DHAVE_CONFIG_H -I. -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII -I../../.. -I../../../src/frontend/SageIII -I/home/dquinlan/ROSE/svn-rose/src -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII/astFixup -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII/astPostProcessing -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII/astMerge -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII/astVisualization -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII/astFileIO -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII/sageInterface -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII/virtualCFG -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII/astTokenStream -I/home/dquinlan/ROSE/svn-rose/src/frontend/SageIII/astHiddenTypeAndDeclarationLists -I../../../src/frontend/SageIII/astFileIO -I/home/dquinlan/ROSE/svn-rose/src/frontend/OpenFortranParser_SAGE_Connection -I/home/dquinlan/ROSE/svn-rose/src/frontend/PHPFrontend -I/home/dquinlan/ROSE/svn-rose/src/frontend/BinaryDisassembly -I/home/dquinlan/ROSE/svn-rose/src/frontend/Disassemblers -I/home/dquinlan/ROSE/svn-rose/src/frontend/ExecFormats -I/home/dquinlan/ROSE/svn-rose/src/backend/unparser -I/home/dquinlan/ROSE/svn-rose/src/backend/unparser/formatSupport -I/home/dquinlan/ROSE/svn-rose/src/backend/unparser/languageIndependenceSupport -I/home/dquinlan/ROSE/svn-rose/src/backend/unparser/CxxCodeGeneration -I/home/dquinlan/ROSE/svn-rose/src/backend/unparser/FortranCodeGeneration -I/home/dquinlan/ROSE/svn-rose/src/backend/unparser/PHPCodeGeneration -I/home/dquinlan/ROSE/svn-rose/src/backend/asmUnparser -I/home/dquinlan/ROSE/svn-rose/src/util -I/home/dquinlan/ROSE/svn-rose/src/util/support -I/home/dquinlan/ROSE/svn-rose/src/util/graphs -I/home/dquinlan/ROSE/svn-rose/src/util/stringSupport -I/home/dquinlan/ROSE/svn-rose/src/util/commandlineProcessing -I/home/dquinlan/ROSE/svn-rose/src/midend/abstractHandle -I/home/dquinlan/ROSE/svn-rose/src/midend/astDiagnostics -I/home/dquinlan/ROSE/svn-rose/src/midend/astInlining -I/home/dquinlan/ROSE/svn-rose/src/midend/astOutlining -I/home/dquinlan/ROSE/svn-rose/src/midend/astProcessing -I/home/dquinlan/ROSE/svn-rose/src/midend/astQuery -I/home/dquinlan/ROSE/svn-rose/src/midend/astRewriteMechanism -I/home/dquinlan/ROSE/svn-rose/src/midend/astUtil/annotation -I/home/dquinlan/ROSE/svn-rose/src/midend/astUtil/astInterface -I/home/dquinlan/ROSE/svn-rose/src/midend/astUtil/astSupport -I/home/dquinlan/ROSE/svn-rose/src/midend/astUtil/symbolicVal -I/home/dquinlan/ROSE/svn-rose/src/midend/binaryAnalyses -I/home/dquinlan/ROSE/svn-rose/src/midend/binaryAnalyses/dataflowanalyses -I/home/dquinlan/ROSE/svn-rose/src/midend/binaryAnalyses/graph -I/home/dquinlan/ROSE/svn-rose/src/midend/binaryAnalyses/instructionSemantics -I/home/dquinlan/ROSE/svn-rose/src/midend/loopProcessing/computation -I/home/dquinlan/ROSE/svn-rose/src/midend/loopProcessing/depGraph -I/home/dquinlan/ROSE/svn-rose/src/midend/loopProcessing/depInfo -I/home/dquinlan/ROSE/svn-rose/src/midend/loopProcessing/driver -I/home/dquinlan/ROSE/svn-rose/src/midend/loopProcessing/outsideInterface -I/home/dquinlan/ROSE/svn-rose/src/midend/loopProcessing/prepostTransformation -I/home/dquinlan/ROSE/svn-rose/src/midend/ompLowering -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/CFG -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/CallGraphAnalysis -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/OAWrap -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/dataflowAnalysis -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/defUseAnalysis -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/distributedMemoryAnalysis -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/dominanceAnalysis -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/graphAnalysis -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/pointerAnal -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/staticInterproceduralSlicing -I/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/valuePropagation -I/home/dquinlan/ROSE/svn-rose/src/midend/programTransformation/partialRedundancyElimination -I/home/dquinlan/ROSE/svn-rose/src/midend/programTransformation/finiteDifferencing -I/home/dquinlan/ROSE/svn-rose/src/midend/programTransformation/functionCallNormalization -I/home/dquinlan/ROSE/svn-rose/src/midend/programTransformation/constantFolding -I/home/dquinlan/ROSE/svn-rose/src/midend/programTransformation/implicitCodeGeneration -I/home/dquinlan/ROSE/svn-rose/src/midend/programTransformation/runtimeTransformation -I/home/dquinlan/ROSE/svn-rose/src/roseSupport -I/home/dquinlan/ROSE/svn-rose/src/3rdPartyLibraries/MSTL -I/home/dquinlan/ROSE/svn-rose/src/3rdPartyLibraries/libharu-2.1.0/include -I../../../src/3rdPartyLibraries/libharu-2.1.0/include -I/home/dquinlan/ROSE/svn-rose/src/3rdPartyLibraries/qrose/Framework -I/home/dquinlan/ROSE/svn-rose/src/3rdPartyLibraries/qrose/Widgets -I/home/dquinlan/ROSE/svn-rose/projects/dataBase -I/home/dquinlan/ROSE/svn-rose/src/util/graphs -I/home/dquinlan/ROSE/svn-rose/src/midend/astUtil/astInterface -I/home/dquinlan/ROSE/svn-rose/libltdl -I/home/dquinlan/local/boost_1_37_0_installTree-gxx-4.2.2/include/boost-1_37 -DBOOST_REGEX_MATCH_EXTRA -I/usr/apps/java/jdk1.5.0_11/bin/../include -I/usr/apps/java/jdk1.5.0_11/bin/../include/linux -fPIC -DYY_NO_TOP_STATE -DYY_NO_POP_STATE -c Cxx_Grammar.C -fPIC -DPIC -o .libs/Cxx_Grammar.o >& Cxx_Grammar.verbose_output
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
