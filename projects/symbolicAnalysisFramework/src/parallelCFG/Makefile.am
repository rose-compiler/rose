include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafpcfgAnalyses.la
libSafpcfgAnalyses_la_SOURCES = pCFG.C \
				MPIDepAnalysis.C \
				pcfgAnnotations.C \
				pCFGIteratorTransfer.C \
				pCFGIterator.C

include_HEADERS = pCFG.h pCFGIterator.h pCFGIteratorTransfer.h pcfgAnnotations.h MPIDepAnalysis.h

noinst_PROGRAMS = pcfgiterator
pcfgiterator_SOURCES = pcfgIteratorSelfTest.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) \
	$(SAF_BUILD_ROOT)/src/analysis/libSafanalysis.la \
	$(SAF_BUILD_ROOT)/src/simpleAnalyses/libSafsimpleAnalyses.la \
	$(SAF_BUILD_ROOT)/src/ParallelControlFlowGraph/libSafpcfgAnalyses.la \
	$(SAF_BUILD_ROOT)/src/lattice/libSaflattice.la \
	$(SAF_BUILD_ROOT)/src/common/libSafcommon.la \
	$(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
	$(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
	$(SAF_BUILD_ROOT)/src/state/libSafstate.la \
	$(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
	$(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la \
	$(SAF_BUILD_ROOT)/src/external/libSafexternal.la

# ----------test program -------------------
#noinst_PROGRAMS = selfTest
#selfTest_SOURCES = selfTest.C
#selfTest_LDADD = $(SAF_BUILD_ROOT)/src/common/libSafcommon.la \
#
#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#check-local:selfTest
#	./selfTest test_example.c
#
#EXTRA_DIST =  test_example.c
