include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
# strange error for corrupted mesgExpr.lo, disabling parallel build for this target
.NOTPARALLEL:libSafmpiAnal.la 
noinst_LTLIBRARIES = libSafmpiAnal.la
libSafmpiAnal_la_SOURCES = \
                   mesgExpr.C \
                   rankDepAnalysis.C \
                   contProcMatchAnalysis.C  \
                   mesgBuf.C \
                   pCFG.C \
		   procSet.C \
                   ranknpDepIfMeet.C

include_HEADERS = contProcMatchAnalysis.h  mesgBuf.h  mesgExpr.h  pCFG.h procSet.h  rankDepAnalysis.h  ranknpDepIfMeet.h
# ----------test program -------------------
#.NOTPARALLEL: commTopoAnalTest  selfTest 
noinst_PROGRAMS = commTopoAnalTest  selfTest
selfTest_SOURCES = selfTest.C
commTopoAnalTest_SOURCES = commTopoAnalTest.C
LDADD = \
         $(SAF_BUILD_ROOT)/src/mpiAnal/libSafmpiAnal.la \
         $(SAF_BUILD_ROOT)/src/lattice/libSaflattice.la \
         $(SAF_BUILD_ROOT)/src/state/libSafstate.la \
         $(SAF_BUILD_ROOT)/src/common/libSafcommon.la \
         $(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
         $(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
         $(SAF_BUILD_ROOT)/src/analysis/libSafanalysis.la \
         $(SAF_BUILD_ROOT)/src/simpleAnalyses/libSafsimpleAnalyses.la \
         $(SAF_BUILD_ROOT)/src/external/libSafexternal.la \
         $(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
         $(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la

LDADD += $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local:selfTest
	./selfTest $(srcdir)/test_example.c

EXTRA_DIST =  fanExchange.C fanoutBcast.C fanInGather.C pCFG_Partitioned.C
clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT


