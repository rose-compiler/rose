include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs
# ----------test program -------------------
#SAF_BUILD_ROOT = $(top_builddir)/projects/symbolicAnalysisFramework
noinst_PROGRAMS = factPlacer latticePlacer scalarFWDataflow pathSensitive genInterestingCFG genCFG
# runtimeBoundAnalysis.C  scalarFWDataflow_example.C
factPlacer_SOURCES = factPlacer.C
latticePlacer_SOURCES = latticePlacer.C
scalarFWDataflow_SOURCES = scalarFWDataflow.C scalarFWDataflow.h
pathSensitive_SOURCES = pathSensitive.C pathSensitive.h
genInterestingCFG_SOURCES = genInterestingCFG.C
genCFG_SOURCES = genCFG.C

# Liao 10/7/2010
# the order of some la files matter:
# e.g. lattice and analysis.la should be listed first
LDADD = \
         $(SAF_BUILD_ROOT)/src/common/libSafcommon.la \
         $(SAF_BUILD_ROOT)/src/analysis/libSafanalysis.la \
         $(SAF_BUILD_ROOT)/src/lattice/libSaflattice.la \
         $(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
         $(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
         $(SAF_BUILD_ROOT)/src/simpleAnalyses/libSafsimpleAnalyses.la \
         $(SAF_BUILD_ROOT)/src/state/libSafstate.la \
         $(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
         $(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la \
         $(SAF_BUILD_ROOT)/src/external/libSafexternal.la

LDADD += $(LIBS_WITH_RPATH) $(ROSE_LIBS)

genInterestingCFG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
genCFG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local:factPlacer latticePlacer scalarFWDataflow pathSensitive
	./factPlacer    $(srcdir)/test_examples/factPlacer_example.cpp > factPlacer_out
# This test passes all but make distcheck for gcc 4.1.2	
#	./latticePlacer $(srcdir)/test_examples/latticePlacer_example.c > latticePlacer_out
# scalarFWDataflow_example.c is not compilable , Liao
#	./scalarFWDataflow $(srcdir)/test_examples/scalarFWDataflow_example.c > scalarFWDataflow_out
# assertion failure	
#	./pathSensitive $(srcdir)/test_examples/pathSensitive_example.cpp > pathSensitive_out

EXTRA_DIST =  test_examples 
clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT *_out


