include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

INCLUDES = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libastprocessingSources = \
   AstPDFGeneration.C AstNodeVisitMapping.C AstTextAttributesHandling.C \
   AstDOTGeneration.C AstProcessing.C AstSimpleProcessing.C Ast.C \
   AstNodePtrs.C AstSuccessorsSelectors.C AstAttributeMechanism.C \
   AstReverseSimpleProcessing.C AstRestructure.C AstClearVisitFlags.C \
   AstTraversal.C AstCombinedSimpleProcessing.C \
   AstSharedMemoryParallelSimpleProcessing.C

# Removed AstTests.C AstStatistics.C 

# lib_LTLIBRARIES = libastprocessing.a
# libastprocessing_a_SOURCES  = $(libastprocessingSources)
# libastprocessing_a_LIBADD  = $(CXX_TEMPLATE_OBJECTS)
# libastprocessing_a_DEPENDENCIES  = $(libastprocessing_a_SOURCES)

noinst_LTLIBRARIES = libastprocessing.la
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
   AstSimpleProcessing.h AstTraverseToRoot.h AstNodePtrs.h \
   AstSuccessorsSelectors.h AstReverseProcessing.h \
   AstReverseSimpleProcessing.h Ast.h AstRestructure.h AstClearVisitFlags.h \
   AstTraversal.h AstCombinedProcessing.h AstCombinedProcessingImpl.h \
   AstCombinedSimpleProcessing.h StackFrameVector.h AstSharedMemoryParallelProcessing.h \
   AstSharedMemoryParallelProcessingImpl.h AstSharedMemoryParallelSimpleProcessing.h

# Removed AstTests.h AstStatistics.h 

EXTRA_DIST = CMakeLists.txt AstProcessing.docs


test_code_size:
	g++ -E -I$(top_builddir) -I. $(ROSE_INCLUDES) $(srcdir)/AstNodeVisitMapping.C | wc -l





