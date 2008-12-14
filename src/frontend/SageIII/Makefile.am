include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Include the "." to force the current directory to be compiled 
# before the subdirectories.
SUBDIRS = astFileIO astMerge astFixup astPostProcessing sageInterface virtualCFG astTokenStream astHiddenTypeAndDeclarationLists astVisualization

# Force sage.docs to be make by default 
# (if sage.docs.in is modified)
all-local: sage.docs

# INCLUDES = $(ROSE_INCLUDES)
# INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src $(WAVE_INCLUDES)
# INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src
# JJW 6-2-2008: We shouldn't be using anything from EDG outside these two directories
INCLUDES = $(ROSE_INCLUDES) # -I$(top_srcdir)/src/frontend/EDG_SAGE_Connection -I$(top_srcdir)/src/frontend/EDG/EDG_3.3/src

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

# Copy the original lex file and the C++ file generated from it to the distribution
# so that users don't have to find an appropriate version of lex (flex).  Flex should
# be used by developers so that ANSI C code is generated for g++.  This is something
# for developers to worry about in the generation of sufficently portable distributions.
# EXTRA_DIST = preproc.lex preproc.C sageClasses.docs sage.docs.in docs
# EXTRA_DIST = preproc.lex preproc.C astGraphTemplate.C docs
EXTRA_DIST = preproc.ll docs rtiHelpers.h attributeListMap.h \
            advanced_preprocessing_hooks.h rose_attributes_list.h dwarfSupport.h
CLEANFILES = haveRosettaGeneratedSource
DISTCLEANFILES = preproc.cc


# DQ (10/18/2007): Added AST_FILE_IO.C StorageClasses.C
# Some source code is generated (call make in ROSE/src/ROSETTA/src to generate these files)
GENERATED_SOURCE = \
	Cxx_Grammar.h \
	Cxx_GrammarRTI.C \
	Cxx_Grammar.C \
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

haveRosettaGeneratedSource:
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)
	touch haveRosettaGeneratedSource

force-haveRosettaGeneratedSource:
	-rm Cxx_Grammar.h
	$(MAKE) haveRosettaGeneratedSource

$(GENERATED_SOURCE): haveRosettaGeneratedSource

libsage3Sources = \
   rose_attributes_list.C \
   attach_all_info.C \
   attachPreprocessingInfo.C \
   attributeListMap.C \
   manglingSupport.C \
   sageSupport.C \
   fixupCopy_scopes.C \
   fixupCopy_symbols.C \
   fixupCopy_references.C \
   rtiHelpers.C \
   OmpAttribute.C \
   dwarfSupport.C

# $(GENERATED_SOURCE) is handled separately

# DQ (5/27/2007): Files moved or removed
#   generateAST.C
#   AstAttributeDOT.C
#   astGraph.C
#   insert.C
#   stripWrapper.C
#   grammarBaseClass.C

noinst_LTLIBRARIES = libsage3.la

libsage3_la_SOURCES = $(libsage3Sources) preproc.ll rose_paths.h
nodist_libsage3_la_SOURCES = $(GENERATED_SOURCE) $(top_builddir)/rose_config.h

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

#	astFileIO/libastFileIO.la 
#	astFileIO/libastFileIO.la 

# $(CXX_TEMPLATE_OBJECTS)
# libsage3_la_DEPENDENCIES = preproc.o $(GENERATED_OBJECTS) astFixup/*.o astPostProcessing/*.o astMerge/*.o astFileIO/*.o sageInterface/*.o virtualCFG/*.o 

rose_attributes_list.lo : rose_attributes_list.h

# DQ (10/18/2007): Added AST_FILE_IO.h StorageClasses.h
# Removed sla.h (redundant with location in src/util/commandlineProcessing/sla.h)
include_HEADERS = \
   sage3.h rose_attributes_list.h \
   attachPreprocessingInfo.h \
   attach_all_info.h manglingSupport.h C++_include_files.h \
   fixupCopy.h \
   general_token_defs.h rose_paths.h rtiHelpers.h \
   OmpAttribute.h omp.h dwarfSupport.h

nodist_include_HEADERS = \
   Cxx_Grammar.h \
   Cxx_GrammarMemoryPoolSupport.h \
   Cxx_GrammarTreeTraversalAccessEnums.h \
   AST_FILE_IO.h StorageClasses.h \
   AstQueryMemoryPool.h \
   astFileIO/AstSpecificDataManagingClass.h

BUILT_SOURCES = $(GENERATED_SOURCE)

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

distclean-local:
	rm -rf Templates.DB

removeHashLineDirectivesFromGeneratedCode:
	cp Cxx_Grammar.h  Cxx_Grammar.h.withLineDirectives
	grep -v "#line" Cxx_Grammar.h > Cxx_Grammar.h.withoutLineDirectives
	cp Cxx_Grammar.h.withoutLineDirectives Cxx_Grammar.h









