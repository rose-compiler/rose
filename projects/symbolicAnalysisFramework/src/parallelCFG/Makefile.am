include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

SAF_SRC_ROOT=$(top_srcdir)/src/midend/programAnalysis/genericDataflow/

# To keep compile commands tidy, please ONLY put required include flags here.                                                                
XXX_INCLUDES = $(ROSE_INCLUDES) \
           -I$(SAF_SRC_ROOT)/include \
           -I$(SAF_SRC_ROOT)/src/analysis \
           -I$(SAF_SRC_ROOT)/src/arrIndexLabeler \
           -I$(SAF_SRC_ROOT)/src/cfgUtils \
           -I$(SAF_SRC_ROOT)/src/chkptRangeAnalysis \
           -I$(SAF_SRC_ROOT)/src/common \
           -I$(SAF_SRC_ROOT)/src/external \
           -I$(SAF_SRC_ROOT)/src/lattice \
           -I$(SAF_SRC_ROOT)/src/mpiAnal \
           -I$(SAF_SRC_ROOT)/src/ompAnal \
           -I$(SAF_SRC_ROOT)/src/rwAccessLabeler \
           -I$(SAF_SRC_ROOT)/src/simpleAnalyses \
           -I$(SAF_SRC_ROOT)/src/state \
           -I$(SAF_SRC_ROOT)/src/unionFind \
           -I$(SAF_SRC_ROOT)/src/varBitVector \
           -I$(SAF_SRC_ROOT)/src/variables \
           -I$(SAF_SRC_ROOT)/src/varLatticeVector

INCLUDES = $(ROSE_INCLUDES) \
           -I$(srcdir) \
           -I$(SAF_SRC_ROOT) \
           -I$(SAF_SRC_ROOT)/analysis \
           -I$(SAF_SRC_ROOT)/arrIndexLabeler \
           -I$(SAF_SRC_ROOT)/cfgUtils \
           -I$(SAF_SRC_ROOT)/lattice \
           -I$(SAF_SRC_ROOT)/rwAccessLabeler \
           -I$(SAF_SRC_ROOT)/simpleAnalyses \
           -I$(SAF_SRC_ROOT)/state \
           -I$(SAF_SRC_ROOT)/variables


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

LDADD = $(ROSE_LIBS) libSafpcfgAnalyses.la

#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) \
#	$(SAF_BUILD_ROOT)/src/parallelCFG/libSafpcfgAnalyses.la
#	$(SAF_BUILD_ROOT)/src/analysis/libSafanalysis.la \
#	$(SAF_BUILD_ROOT)/src/simpleAnalyses/libSafsimpleAnalyses.la \
#	$(SAF_BUILD_ROOT)/src/lattice/libSaflattice.la \
#	$(SAF_BUILD_ROOT)/src/common/libSafcommon.la \
#	$(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
#	$(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
#	$(SAF_BUILD_ROOT)/src/state/libSafstate.la \
#	$(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
#	$(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la \
#	$(SAF_BUILD_ROOT)/src/external/libSafexternal.la

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
