
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

MFB_REL_PATH=src/midend/MFB
MFB_INC_PATHS=-I$(top_srcdir)/src/midend/MFB/include
MFB_LIB_PATHS=-L$(top_builddir)/$(MFB_REL_PATH)/lib/sage -L$(top_builddir)/$(MFB_REL_PATH)/lib/utils
MFB_LIBS=-lMFB-Sage -lMFB-Utils

RAT_REL_PATH=projects/TemplateAnalysis
RAT_ORG_PATH=ROSE/Analysis/Template
RAT_INC_PATHS=-I$(top_srcdir)/$(RAT_REL_PATH)/include
RAT_LIB_PATH=-L$(top_builddir)/$(RAT_REL_PATH)/lib/$(RAT_ORG_PATH)
RAT_LIBS=-lROSE-Analysis-Templates

PROP_REL_PATH=projects/TemplateAnalysis
PROP_ORG_PATH=ROSE/proposed
PROP_INC_PATHS=-I$(top_srcdir)/$(PROP_REL_PATH)/include
PROP_LIB_PATH=-L$(top_builddir)/$(PROP_REL_PATH)/lib/$(PROP_ORG_PATH)
PROP_LIBS=-lROSE-Proposed

bin_PROGRAMS=mfbEnv

mfbEnv_SOURCES=mfb-environment.cpp
mfbEnv_CXXFLAGS=-g $(MFB_INC_PATHS) $(RAT_INC_PATHS) $(PROP_INC_PATHS) $(ROSE_INCLUDES)
mfbEnv_LDADD=$(MFB_LIB_PATHS) $(MFB_LIBS) $(RAT_LIB_PATH) $(RAT_LIBS) $(PROP_LIB_PATH) $(PROP_LIBS) $(LIBS_WITH_RPATH) $(ROSE_LIBS)

