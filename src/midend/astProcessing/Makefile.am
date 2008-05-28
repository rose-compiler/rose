include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (11/19/2007): Removed the use of Coco
# SUBDIRS = CocoPreprocessor
SUBDIRS =

INCLUDES = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libastprocessingSources = \
   AstPDFGeneration.C AstNodeVisitMapping.C AstTextAttributesHandling.C \
   AstDOTGeneration.C AstProcessing.C AstSimpleProcessing.C Ast.C \
   AstNodePtrs.C AstSuccessorsSelectors.C \
   AstReverseSimpleProcessing.C AstRestructure.C AstClearVisitFlags.C \
   AstTraversal.C AstCombinedSimpleProcessing.C \
   AstSharedMemoryParallelSimpleProcessing.C

# DQ (1/3/2008): I think we can remove this Coco dependent file.
# sgnodep.cpp AgProcessing.C AgDefaultError.C 

# DQ (11/19/2007): Removed the use of Coco
# AstNodeScanner.C

# Removed AstTests.C AstStatistics.C 

# lib_LTLIBRARIES = libastprocessing.a
# libastprocessing_a_SOURCES  = $(libastprocessingSources)
# libastprocessing_a_LIBADD  = $(CXX_TEMPLATE_OBJECTS)
# libastprocessing_a_DEPENDENCIES  = $(libastprocessing_a_SOURCES)

if DEV_LIBS
lib_LTLIBRARIES = libastprocessing.la
else
noinst_LTLIBRARIES = libastprocessing.la
endif
libastprocessing_la_SOURCES = $(libastprocessingSources)
libastprocessing_la_LIBADD = $(CXX_TEMPLATE_OBJECTS)
libastprocessing_la_DEPENDENCIES = $(libastprocessing_a_SOURCES)

# $(GENERATED_SOURCE):
#	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = \
   AstPDFGeneration.h AstNodeVisitMapping.h AstAttributeMechanism.h \
   AstTextAttributesHandling.h AstDOTGeneration.h AstProcessing.h \
   AstSimpleProcessing.h AstTraverseToRoot.h AstNodePtrs.h AstNodeScanner.h \
   AstSuccessorsSelectors.h AstReverseProcessing.h \
   AstReverseSimpleProcessing.h Ast.h AstRestructure.h AstClearVisitFlags.h \
   AstTraversal.h AstCombinedProcessing.h AstCombinedProcessingImpl.h \
   AstCombinedSimpleProcessing.h StackFrameVector.h AstSharedMemoryParallelProcessing.h \
   AstSharedMemoryParallelProcessingImpl.h AstSharedMemoryParallelSimpleProcessing.h

# Removed AstTests.h AstStatistics.h 

# DQ (1/3/2007): Removed use of Coco parser generator.
# AgProcessing.h AgProcessing.C AgDefaultError.h sgnodec.hpp sgnodep.hpp sgnodes.hpp sgnodee.hpp 

EXTRA_DIST = AstProcessing.docs







