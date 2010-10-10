include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Include the "." to force the current directory to be compiled 
# before the subdirectories.
SUBDIRS = astFileIO astMerge astFixup astPostProcessing sageInterface virtualCFG astTokenStream astHiddenTypeAndDeclarationLists astVisualization

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

# omplexer.cc needs a header generaetd from ompparser.yy
omplexer.lo: ompparser.h omplexer.cc
ompparser.h: ompparser.cc

# Force sage.docs to be make by default 
# (if sage.docs.in is modified)
all-local: roseh sage.docs 


# INCLUDES = $(ROSE_INCLUDES)
# INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src $(WAVE_INCLUDES)
# INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src
# JJW 6-2-2008: We shouldn't be using anything from EDG outside these two directories
# INCLUDES = $(ROSE_INCLUDES) # -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src
INCLUDES = $(ROSE_INCLUDES)

# DQ (3/13/2010): sageSupport.C requires the OFP fortran source files to be made available.
if ROSE_USE_OPEN_FORTRAN_PARSER
   INCLUDES += -I${top_builddir}/src/3rdPartyLibraries/fortran-parser
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
EXTRA_DIST = CMakeLists.txt preproc-c.ll docs rtiHelpers.h attributeListMap.h \
             advanced_preprocessing_hooks.h rose_attributes_list.h dwarfSupport.h \
             omplexer.cc ompparser.cc ompparser.h sage3basic.hhh fileoffsetbits.h rosedll.h
CLEANFILES = haveRosettaGeneratedSource

DISTCLEANFILES = preproc-c.cc ompparser.cc ompparser.h omplexer.cc

# Some source code is generated (call make in ROSE/src/ROSETTA/src to generate these files)
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

# DQ (4/23/2009): This file depends upon other files
StorageClasses.lo: $(srcdir)/astFileIO/StorageClassMemoryManagement.C astFileIO/SourcesOfIRNodesAstFileIOSupport.C

haveRosettaGeneratedSource:
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)
	touch haveRosettaGeneratedSource

force-haveRosettaGeneratedSource:
	-rm Cxx_Grammar.h
	$(MAKE) haveRosettaGeneratedSource

$(GENERATED_SOURCE): haveRosettaGeneratedSource

libsage3Sources = \
   rose_attributes_list.C \
   attachPreprocessingInfo.C \
   attachPreprocessingInfoTraversal.C \
   attributeListMap.C \
   manglingSupport.C \
   sageSupport.C \
   fixupCopy_scopes.C \
   fixupCopy_symbols.C \
   fixupCopy_references.C \
   rtiHelpers.C \
   OmpAttribute.C \
   ompFortranParser.C \
   dwarfSupport.C \
   rose_graph_support.C

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
libsage3_la_SOURCES = $(libsage3Sources) preproc-c.ll omplexer.ll ompparser.yy

# DQ (3/8/2009): I think that the dependence upon $(top_builddir)/rose_config.h 
# is what was forcing a lot of files to be compiled after reruning configure.
# nodist_libsage3_la_SOURCES = $(GENERATED_SOURCE) $(top_builddir)/rose_config.h
nodist_libsage3_la_SOURCES = $(GENERATED_SOURCE)


if ROSE_USE_BOOST_WAVE
libsage3_la_LIBADD  = \
	astFixup/libastFixup.la \
	astPostProcessing/libastPostProcessing.la \
	astMerge/libastMerge.la \
	sageInterface/libsageInterface.la \
	astTokenStream/libastTokenStream.la \
	virtualCFG/libvirtualCFG.la \
	astHiddenTypeAndDeclarationLists/libastHiddenTypeAndDeclarationLists.la \
   astVisualization/libastVisualization.la
else
libsage3_la_LIBADD  = \
	astFixup/libastFixup.la \
	astPostProcessing/libastPostProcessing.la \
	astMerge/libastMerge.la \
	sageInterface/libsageInterface.la \
	virtualCFG/libvirtualCFG.la \
	astHiddenTypeAndDeclarationLists/libastHiddenTypeAndDeclarationLists.la \
   astVisualization/libastVisualization.la
endif

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
roseh:
#	DQ (2/10/2009): Rush for the off switch on precompiled header files, comment out to skip use of PCH.
#       TPS (23 Feb 2009): Hardcoded -fpic and -DPIC for now because 
#       for some reason it is used by most files although not specified on configure
	@echo "  COMPILE sage3basic.h.gch"
	@$(CXX)  -fPIC -DPIC $(CXXFLAGS) $(CFLAGS) $(CPPFLAGS) -g -o sage3basic.h.gch $(top_srcdir)/src/frontend/SageIII/sage3basic.h -I$(top_builddir) $(ROSE_INCLUDES) 
endif
else
# default, do nothing.
roseh:

endif

test_roseh: roseh

# DQ (10/18/2007): Added AST_FILE_IO.h StorageClasses.h
# Removed sla.h (redundant with location in src/util/commandlineProcessing/sla.h)
include_HEADERS = \
   sage3.h sage3basic.h rose_attributes_list.h \
   attachPreprocessingInfo.h \
   attachPreprocessingInfoTraversal.h \
   attach_all_info.h manglingSupport.h C++_include_files.h \
   fixupCopy.h \
   general_token_defs.h rtiHelpers.h \
   OmpAttribute.h omp.h dwarfSupport.h \
   omp_lib_kinds.h omp_lib.h sage3basic.hhh rosedefs.h  fileoffsetbits.h rosedll.h

# DQ (4/5/2009): Moved rose_paths.h to src/util (where the source file is located).
nodist_include_HEADERS = \
   Cxx_Grammar.h \
   Cxx_GrammarMemoryPoolSupport.h \
   Cxx_GrammarTreeTraversalAccessEnums.h \
   AST_FILE_IO.h StorageClasses.h \
   AstQueryMemoryPool.h \
   astFileIO/AstSpecificDataManagingClass.h

BUILT_SOURCES = $(GENERATED_SOURCE) ompparser.cc ompparser.h omplexer.cc


# DQ (3/13/2010): Extract the required OFP source files needed for ROSE Fortran support (in sageSupport.C).
sageSupport.C: extract_rose_required_ofp_source_files

# DQ (10/9/2010): We are now trying to properly represent the dependences so avoid redundant
# extraction of the OFP source files used within ROSE. Using token.c as a representative file.
extract_rose_required_ofp_source_files:
	cd ${top_builddir}/src/3rdPartyLibraries/fortran-parser; $(MAKE) extract_rose_required_ofp_source_files
#	cd ${top_builddir}/src/3rdPartyLibraries/fortran-parser; $(MAKE) token.c


# DQ (9/9/2007): Put C++_include_files.h back into place
# since it is a convinient mechanism to handle included headers 
# in the Cxx_Grammar.C file (a ROSETTA generated file).
# DQ (5/27/2007): Files moved or removed
# grammarBaseClass.h 
# AstAttributeDOT.h
# astGraph.h
# astGraphTemplate.C
# sageSupport.h
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












