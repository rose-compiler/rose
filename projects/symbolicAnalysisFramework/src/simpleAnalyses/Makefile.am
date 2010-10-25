include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafsimpleAnalyses.la
libSafsimpleAnalyses_la_SOURCES = affineInequality.C  divAnalysis.C  nodeConstAnalysis.C \
                          placeUIDs.C  printAnalysisStates.C  saveDotAnalysis.C \
			  sgnAnalysis.C
include_HEADERS = affineInequality.h  divAnalysis.h  nodeConstAnalysis.h  placeUIDs.h  \
                  printAnalysisStates.h  saveDotAnalysis.h  sgnAnalysis.h

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
