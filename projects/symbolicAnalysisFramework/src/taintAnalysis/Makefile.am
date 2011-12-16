include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ: This should be handled differently.  No analysis should not have its own makefile include file.
# include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# SAF_SRC_ROOT   = $(top_srcdir)/projects/symbolicAnalysisFramework
SAF_SRC_ROOT   = $(top_srcdir)/src/midend/programAnalysis/genericDataflow/

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

bin_PROGRAMS = taintAnalysisTest

taintAnalysisTest_SOURCES = taintAnalysis.C constantPropagation.C taintAnalysisSelfTest.C

LDADD = $(ROSE_LIBS)

