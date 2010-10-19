include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafanalysis.la
libSafanalysis_la_SOURCES = analysis.C  analysisCommon.C  partitionedAnalysis.C
include_HEADERS = analysisCommon.h  analysis.h  dataflow.h  partitionedAnalysis.h

EXTRA_DIST = 
