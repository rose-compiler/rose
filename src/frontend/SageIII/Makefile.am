include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

include $(srcdir)/sage_support/Makefile_variables

#
# Don't use $(srcdir) in conjunction with [subdir-objects], otherwise
# object files will be built in the source tree. Therefore, we need to
# use relative paths.
#
#mptPartialRedundancyEliminationPath=$(srcdir)/partialRedundancyElimination#
fSageSupportPath=sage_support#

# DQ (6/25/2011): Readded this directory so that we can support the older behavior as a default
# although this will likely fail at least some of the nearly one hundred new name qualification tests.
# DQ (6/22/2011): I think that the astHiddenTypeAndDeclarationLists directory is no longer required in ROSE.
# SUBDIRS = astFileIO astMerge astFixup astPostProcessing sageInterface virtualCFG astTokenStream astHiddenTypeAndDeclarationLists astVisualization astFromString
# SUBDIRS = astFileIO astMerge astFixup astPostProcessing sageInterface virtualCFG astTokenStream astVisualization astFromString
# SUBDIRS = astFileIO astMerge astFixup astPostProcessing sageInterface virtualCFG astTokenStream astHiddenTypeAndDeclarationLists astVisualization astFromString includeDirectivesProcessing

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   SUBDIRS = astFixup astPostProcessing sageInterface astVisualization virtualCFG includeDirectivesProcessing
else
   SUBDIRS = astFileIO astMerge astFixup astPostProcessing sageInterface virtualCFG astTokenStream astHiddenTypeAndDeclarationLists astVisualization astFromString includeDirectivesProcessing
endif

# DQ (12/29/2009): This is only used to handle generated code as smaller files.
SUBDIRS += GENERATED_CODE_DIRECTORY_Cxx_Grammar

# DQ (5/2/2009): This supports the optimization of the new 
# Graph IR nodes (makes about 25% improvement, I think).
# ROSE_OPTIMIZATION_LEVEL = -O3
ROSE_OPTIMIZATION_LEVEL =
# ROSE_VERBOSE = -rose:verbose 2

# Liao, 3/31/2009, moved the OpenMP parser here
#Liao, 10/27/2008. I decide to parse OpenMP pragma string here
# Bison flags
AM_YFLAGS = -d
# Flex flags
AM_LFLAGS =
# avoid unused Flex functions which are automatically generated
#CXXFLAGS+= -DYY_NO_TOP_STATE -DYY_NO_POP_STATE $(ROSE_OPTIMIZATION_LEVEL)

CXXFLAGS += $(ROSE_OPTIMIZATION_LEVEL) $(ROSE_VERBOSE)

# DQ (11/14/2011): Add debug support when doing selective frontend development.
# This cause the link time to incread about 4X.
if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
# CXXFLAGS += -g
endif

# omplexer.cc needs a header generaetd from ompparser.yy
omplexer.lo: ompparser.h omplexer.cc
ompparser.h: ompparser.cc
	if test -f ompparser.hh; then $(LN_S) -f ompparser.hh ompparser.h; fi;

# Force sage.docs to be make by default 
# (if sage.docs.in is modified)
all-local: roseh sage.docs 


# AM_CPPFLAGS = $(ROSE_INCLUDES)
# AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src $(WAVE_INCLUDES)
# AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src
# JJW 6-2-2008: We shouldn't be using anything from EDG outside these two directories
# AM_CPPFLAGS = $(ROSE_INCLUDES) # -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src
AM_CPPFLAGS = $(ROSE_INCLUDES)

# DQ (3/13/2010): sage_support.cpp requires the OFP fortran source files to be made available.
# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   AM_CPPFLAGS += -I${top_builddir}/src/3rdPartyLibraries/fortran-parser
endif

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

# Copy the original lex file and the C++ file generated from it to the distribution
# so that users don't have to find an appropriate version of lex (flex).  Flex should
# be used by developers so that ANSI C code is generated for g++.  This is something
# for developers to worry about in the generation of sufficently portable distributions.
# EXTRA_DIST = preproc.lex preproc.C sageClasses.docs sage.docs.in docs
# EXTRA_DIST = preproc.lex preproc.C astGraphTemplate.C docs
# EXTRA_DIST = preproc.ll docs rtiHelpers.h attributeListMap.h

# DQ (9/29/2009): Renamed preproc.ll to preproc-c.ll in SVN.
# This will hopefull avoid a linking problem where multiple files
# are called preproc.o (from here and from EDG).
# Rasmussen (04/17/2019): Support for ATerms has been deprecated (atermSupport.h removed)
EXTRA_DIST = CMakeLists.txt preproc-c.ll docs rtiHelpers.h attributeListMap.h \
             advanced_preprocessing_hooks.h rose_attributes_list.h dwarfSupport.h \
             omplexer.cc ompparser.cc ompparser.h sage3basic.hhh fileoffsetbits.h rosedll.h \
	     ompAstConstruction.h PolicyControl.txt Utf8.h
CLEANFILES = haveRosettaGeneratedSource

DISTCLEANFILES = preproc-c.cc ompparser.cc ompparser.h omplexer.cc

# Some source code is generated (call make in ROSE/src/ROSETTA/src to generate these files)
if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
GENERATED_SOURCE = \
	Cxx_Grammar.h \
	Cxx_Grammar.C \
	Cxx_GrammarMemoryPoolSupport.C \
	AstQueryMemoryPool.h \
	Cxx_GrammarReturnClassHierarchySubTree.C \
	Cxx_GrammarTreeTraversalSuccessorContainer.C \
	Cxx_GrammarTreeTraversalAccessEnums.h \
	Cxx_GrammarVariantEnumNames.C \
	Cxx_GrammarSerialization.C \
	Cxx_GrammarSerialization.h \
	Cxx_GrammarNewAndDeleteOperators.C \
	Cxx_GrammarCopyMemberFunctions.C \
	Cxx_GrammarTraverseMemoryPool.C \
	Cxx_GrammarCheckingIfDataMembersAreInMemoryPool.C \
	Cxx_GrammarMemoryPoolSupport.h \
	Cxx_GrammarReturnDataMemberPointers.C \
	Cxx_GrammarProcessDataMemberReferenceToPointers.C \
	Cxx_GrammarNewConstructors.C \
	Cxx_GrammarGetChildIndex.C
else
GENERATED_SOURCE = \
	Cxx_Grammar.h \
	Cxx_Grammar.C \
	Cxx_GrammarRTI.C \
	Cxx_GrammarMemoryPoolSupport.C \
	AstQueryMemoryPool.h \
	Cxx_GrammarReturnClassHierarchySubTree.C \
	Cxx_GrammarTreeTraversalSuccessorContainer.C \
	Cxx_GrammarTreeTraversalAccessEnums.h \
	Cxx_GrammarVariantEnumNames.C \
	Cxx_GrammarSerialization.C \
	Cxx_GrammarSerialization.h \
	Cxx_GrammarNewAndDeleteOperators.C \
	Cxx_GrammarCopyMemberFunctions.C \
	Cxx_GrammarTraverseMemoryPool.C \
	Cxx_GrammarCheckingIfDataMembersAreInMemoryPool.C \
	Cxx_GrammarMemoryPoolSupport.h \
	AST_FILE_IO.h \
	AST_FILE_IO.C \
	StorageClasses.h \
	StorageClasses.C \
	Cxx_GrammarReturnDataMemberPointers.C \
	Cxx_GrammarProcessDataMemberReferenceToPointers.C \
	Cxx_GrammarNewConstructors.C \
	Cxx_GrammarGetChildIndex.C
endif

# DQ (4/23/2009): This file depends upon other files
StorageClasses.lo: $(srcdir)/astFileIO/StorageClassMemoryManagement.C astFileIO/SourcesOfIRNodesAstFileIOSupport.C

haveRosettaGeneratedSource:
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)
	touch haveRosettaGeneratedSource

force-haveRosettaGeneratedSource:
	-rm Cxx_Grammar.h
	$(MAKE) haveRosettaGeneratedSource

$(GENERATED_SOURCE): haveRosettaGeneratedSource

# Rasmussen (04/17/2019): Support for ATerms has been deprecated as it is no longer needed
# and likely never fully implemented nor tested.  Files remain in the src tree but are no
# longer built.  Files atermSupport.C and nodeBuildFunctionsForAterms.C have been removed
# from libsage3Sources below (in the else clause).

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
libsage3Sources = \
   Utf8.C \
   rose_attributes_list.C \
   attachPreprocessingInfo.C \
   attachPreprocessingInfoTraversal.C \
   attributeListMap.C \
   manglingSupport.C \
   fixupCopy_scopes.C \
   fixupCopy_symbols.C \
   fixupCopy_references.C \
   rose_graph_support.C \
   $(fSageSupport_la_sources)
else
libsage3Sources = \
   Utf8.C \
   rose_attributes_list.C \
   attachPreprocessingInfo.C \
   attachPreprocessingInfoTraversal.C \
   attributeListMap.C \
   manglingSupport.C \
   fixupCopy_scopes.C \
   fixupCopy_symbols.C \
   fixupCopy_references.C \
   rtiHelpers.C \
   OmpAttribute.C \
   ompFortranParser.C \
   ompAstConstruction.cpp \
   dwarfSupport.C \
   rose_graph_support.C \
   $(fSageSupport_la_sources)
endif

# DQ (2/15/2010): This fails in Hudson tests of ROSE compiling ROSE for code generation, but I can't 
# get it to fail in my own tests.  Skip it for now and see if there are more of these cases.
# attachPreprocessingInfo.C
# if !ROSE_USING_ROSE_CODE_GENERATION
#    libsage3Sources += attachPreprocessingInfo.C
# endif

# $(GENERATED_SOURCE) is handled separately

# DQ (5/27/2007): Files moved or removed
#   generateAST.C
#   AstAttributeDOT.C
#   astGraph.C
#   insert.C
#   stripWrapper.C
#   grammarBaseClass.C

# DQ (12/16/2008): Removed as part of rewrite of CPP and comment handling.
#   attach_all_info.C 

noinst_LTLIBRARIES = libsage3.la

# libsage3_la_SOURCES = $(libsage3Sources) preproc.ll rose_paths.h omplexer.ll ompparser.yy
# libsage3_la_SOURCES = $(libsage3Sources) preproc.ll omplexer.ll ompparser.yy
if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
libsage3_la_SOURCES = $(libsage3Sources) preproc-c.ll
else
libsage3_la_SOURCES = $(libsage3Sources) preproc-c.ll omplexer.ll ompparser.yy
endif

# DQ (3/8/2009): I think that the dependence upon $(top_builddir)/rose_config.h 
# is what was forcing a lot of files to be compiled after reruning configure.
# nodist_libsage3_la_SOURCES = $(GENERATED_SOURCE) $(top_builddir)/rose_config.h
nodist_libsage3_la_SOURCES = $(GENERATED_SOURCE)


if ROSE_USE_BOOST_WAVE

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
libsage3_la_LIBADD  = \
	astFixup/libastFixup.la \
	astPostProcessing/libastPostProcessing.la \
	sageInterface/libsageInterface.la \
   astVisualization/libastVisualization.la \
	virtualCFG/libvirtualCFG.la \
   includeDirectivesProcessing/libincludeDirectivesProcessing.la
else
libsage3_la_LIBADD  = \
	astFixup/libastFixup.la \
	astPostProcessing/libastPostProcessing.la \
	astMerge/libastMerge.la \
	sageInterface/libsageInterface.la \
	astTokenStream/libastTokenStream.la \
	virtualCFG/libvirtualCFG.la \
        astVisualization/libastVisualization.la \
        astFromString/libAstFromString.la \
        astHiddenTypeAndDeclarationLists/libastHiddenTypeAndDeclarationLists.la \
        includeDirectivesProcessing/libincludeDirectivesProcessing.la
endif
else

libsage3_la_LIBADD  = \
	astFixup/libastFixup.la \
	astPostProcessing/libastPostProcessing.la \
	astMerge/libastMerge.la \
	sageInterface/libsageInterface.la \
	virtualCFG/libvirtualCFG.la \
        astVisualization/libastVisualization.la \
        astFromString/libAstFromString.la \
        astHiddenTypeAndDeclarationLists/libastHiddenTypeAndDeclarationLists.la \
        includeDirectivesProcessing/libincludeDirectivesProcessing.la
endif

# DQ (6/25/2011): Readded this directory so that we can support the older behavior as a default
# although this will likely fail at least some of the nearly one hundred new name qualification tests.
# DQ (6/24/2011): Removed now the the name qualification support in a part of src/backend/unparser.
# astHiddenTypeAndDeclarationLists/libastHiddenTypeAndDeclarationLists.la 

if ROSE_USE_SMALLER_GENERATED_FILES
   libsage3_la_LIBADD += GENERATED_CODE_DIRECTORY_Cxx_Grammar/libgeneratedIR.la
endif

rose_attributes_list.lo : rose_attributes_list.h 

if ROSE_PCH
if ROSE_MPI
# TPS (11Feb2009): PCH only works when multithreading is off.
# We should not use PCH and MPI together.
roseh:

else
# DQ (2/11/2009): This is part of the support for Precompiled Headers (PCH) using GNU g++
roseh_v_at_0 = @
roseh_v_at_1 =
roseh_v_at_  = $(roseh_v_at_0)
roseh_v_at   = $(roseh_v_at_$(V))

roseh_v_echo_0 = @echo " COMPILE sage3basic.h.gch"
roseh_v_echo_1 =
roseh_v_echo_  = $(roseh_v_echo_0)
roseh_v_echo   = $(roseh_v_echo_$(V))

roseh:
#	DQ (2/10/2009): Rush for the off switch on precompiled header files, comment out to skip use of PCH.
#       TPS (23 Feb 2009): Hardcoded -fpic and -DPIC for now because 
#       for some reason it is used by most files although not specified on configure
	$(roseh_v_echo)
	$(roseh_v_at)$(CXX) -fPIC -DPIC $(CXXFLAGS) $(CFLAGS) $(CPPFLAGS) -g -o sage3basic.h.gch \
		$(top_srcdir)/src/frontend/SageIII/sage3basic.h -I$(top_builddir) $(ROSE_INCLUDES) 
endif
else
# default, do nothing.
roseh:

endif

test_roseh: roseh

# DQ (10/18/2007): Added AST_FILE_IO.h StorageClasses.h
# Removed sla.h (redundant with location in src/util/commandlineProcessing/sla.h)
# Rasmussen (04/17/2019): Support for ATerms has been deprecated (atermSupport.h removed)
pkginclude_HEADERS = \
   sage3.h sage3basic.h rose_attributes_list.h \
   attachPreprocessingInfo.h \
   attachPreprocessingInfoTraversal.h \
   attach_all_info.h manglingSupport.h C++_include_files.h \
   fixupCopy.h \
   general_token_defs.h rtiHelpers.h \
   OmpAttribute.h omp.h dwarfSupport.h \
   omp_lib_kinds.h omp_lib.h sage3basic.hhh rosedefs.h  fileoffsetbits.h rosedll.h \
   Cxx_GrammarSerialization.h \
   $(fSageSupport_includeHeaders)

# DQ (4/5/2009): Moved rose_paths.h to src/util (where the source file is located).
nodist_pkginclude_HEADERS = \
   Cxx_Grammar.h \
   Cxx_GrammarMemoryPoolSupport.h \
   Cxx_GrammarTreeTraversalAccessEnums.h \
   AST_FILE_IO.h StorageClasses.h \
   AstQueryMemoryPool.h \
   astFileIO/AstSpecificDataManagingClass.h

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
BUILT_SOURCES = $(GENERATED_SOURCE)
else
BUILT_SOURCES = $(GENERATED_SOURCE) ompparser.cc ompparser.h omplexer.cc
endif

# DQ (9/9/2007): Put C++_include_files.h back into place
# since it is a convinient mechanism to handle included headers 
# in the Cxx_Grammar.C file (a ROSETTA generated file).
# DQ (5/27/2007): Files moved or removed
# grammarBaseClass.h 
# AstAttributeDOT.h
# astGraph.h
# astGraphTemplate.C
# C++_include_files.h

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf Cxx_Grammar* AST_FILE_IO.C StorageClasses.C
	rm -rf AST_FILE_IO.h StorageClasses.h AstQueryMemoryPool.h
	rm -rf generated_abstractcppgrammar.atg
	rm -f sage3basic.h.gch
	rm -f *.binary

distclean-local:
	rm -rf Templates.DB
	rm -f sage3basic.h.gch

removeHashLineDirectivesFromGeneratedCode:
	cp Cxx_Grammar.h  Cxx_Grammar.h.withLineDirectives
	grep -v "#line" Cxx_Grammar.h > Cxx_Grammar.h.withoutLineDirectives
	cp Cxx_Grammar.h.withoutLineDirectives Cxx_Grammar.h












