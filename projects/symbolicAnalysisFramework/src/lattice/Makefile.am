include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSaflattice.la
libSaflattice_la_SOURCES = ConstrGraph.C  intArithLogical.C  lattice.C
include_HEADERS = ConstrGraph.h  intArithLogical.h  latticeFull.h  lattice.h
# ----------test program -------------------
#.NOTPARALLEL:selfTest

noinst_PROGRAMS = selfTest
selfTest_SOURCES = selfTest.C
# the order or la files matters!!
selfTest_LDADD = \
         libSaflattice.la \
         $(SAF_BUILD_ROOT)/src/common/libSafcommon.la \
         $(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
         $(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
         $(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
         $(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la \
         $(SAF_BUILD_ROOT)/src/state/libSafstate.la \
         $(SAF_BUILD_ROOT)/src/analysis/libSafanalysis.la \
         $(SAF_BUILD_ROOT)/src/external/libSafexternal.la \
         $(SAF_BUILD_ROOT)/src/simpleAnalyses/libSafsimpleAnalyses.la

selfTest_LDADD += $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local:selfTest
#	./selfTest test_example.c

EXTRA_DIST =  test.C

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT

