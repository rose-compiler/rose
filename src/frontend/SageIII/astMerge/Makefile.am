include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)


# Some source code is generated (call make in ROSE/src/ROSETTA/src to generate these files)
GENERATED_SOURCE  = ../Cxx_Grammar.C ../Cxx_GrammarTreeTraversalSuccessorContainer.C ../Cxx_GrammarRTI.C
GENERATED_OBJECTS = ../Cxx_Grammar.o ../Cxx_GrammarTreeTraversalSuccessorContainer.o ../Cxx_GrammarRTI.o

$(GENERATED_SOURCE):
	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)

noinst_LTLIBRARIES = libastMerge.la
libastMerge_la_SOURCES      = \
     merge_support.C test_support.C buildMangledNameMap.C buildSetOfFrontendSpecificNodes.C \
     deleteNodes.C fixupTraversal.C nullifyAST.C buildReplacementMap.C collectAssociateNodes.C \
     deleteOrphanNodes.C normalizeTypes.C requiredNodes.C merge.C AstFixParentTraversal.C

libastMerge_la_LIBADD       = 
libastMerge_la_DEPENDENCIES = $(GENERATED_SOURCE)

pkginclude_HEADERS = \
     buildMangledNameMap.h  buildReplacementMap.h  collectAssociateNodes.h  deleteOrphanNodes.h \
     fixupTraversal.h  merge.h  merge_support.h  nullifyAST.h  test_support.h requiredNodes.h AstFixParentTraversal.h


EXTRA_DIST = CMakeLists.txt
