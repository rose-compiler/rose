include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafchkptRangeAnalysis.la
libSafchkptRangeAnalysis_la_SOURCES = chkptRangeAnalysis.C
include_HEADERS = chkptRangeAnalysis.h

# ----------test program -------------------
#.NOTPARALLEL:selfTest

noinst_PROGRAMS = selfTest
selfTest_SOURCES = selfTest.C
selfTest_LDADD = libSafchkptRangeAnalysis.la \
                 $(SAF_BUILD_ROOT)/src/lattice/libSaflattice.la \
                 $(SAF_BUILD_ROOT)/src/common/libSafcommon.la \
                 $(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
                 $(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
                 $(SAF_BUILD_ROOT)/src/state/libSafstate.la \
                 $(SAF_BUILD_ROOT)/src/unionFind/libSafunionFind.la \
                 $(SAF_BUILD_ROOT)/src/varBitVector/libSafvarBitVector.la \
		 $(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
		 $(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la \
                 $(SAF_BUILD_ROOT)/src/analysis/libSafanalysis.la \
                 $(SAF_BUILD_ROOT)/src/simpleAnalyses/libSafsimpleAnalyses.la \
                 $(SAF_BUILD_ROOT)/src/analysis/libSafanalysis.la

selfTest_LDADD += $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local:selfTest
#	./selfTest $(srcdir)/test_example.c
EXTRA_DIST = test_example.c
clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT

