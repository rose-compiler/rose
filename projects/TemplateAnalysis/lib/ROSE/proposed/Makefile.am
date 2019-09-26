
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

MFB_REL_PATH=src/midend/MFB
MFB_INC_PATHS=-I$(top_srcdir)/$(MFB_REL_PATH)/include

RAT_REL_PATH=projects/TemplateAnalysis
RAT_INC_PATHS=-I$(top_srcdir)/$(RAT_REL_PATH)/include

lib_LTLIBRARIES=libROSE-Proposed.la

libROSE_Proposed_la_SOURCES=sage-interface.cpp mfb-json.cpp
libROSE_Proposed_la_CXXFLAGS=-g $(MFB_INC_PATHS) $(RAT_INC_PATHS) $(ROSE_INCLUDES)

