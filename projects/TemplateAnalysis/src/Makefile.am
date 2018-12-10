
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

MFB_REL_PATH=src/midend/MFB
MFB_INC_PATHS=-I$(abs_top_srcdir)/src/midend/MFB/include
MFB_LIB_SAGE=$(abs_top_builddir)/$(MFB_REL_PATH)/lib/sage/libMFB-Sage.la
MFB_LIB_UTILS=$(abs_top_builddir)/$(MFB_REL_PATH)/lib/utils/libMFB-Utils.la

$(MFB_LIB_SAGE):
	$(MAKE) -C $(abs_top_builddir)/$(MFB_REL_PATH)/lib/sage libMFB-Sage.la

$(MFB_LIB_UTILS):
	$(MAKE) -C $(abs_top_builddir)/$(MFB_REL_PATH)/lib/utils libMFB-Utils.la

RAT_REL_PATH=projects/TemplateAnalysis
RAT_ORG_PATH=ROSE/Analysis/Template
RAT_INC_PATHS=-I$(abs_top_srcdir)/$(RAT_REL_PATH)/include
RAT_LIB=$(abs_top_builddir)/$(RAT_REL_PATH)/lib/$(RAT_ORG_PATH)/libROSE-Analysis-Templates.la

$(RAT_LIB):
	$(MAKE) -C $(abs_top_builddir)/$(RAT_REL_PATH)/lib/$(RAT_ORG_PATH) libROSE-Analysis-Templates.la

PROP_REL_PATH=projects/TemplateAnalysis
PROP_ORG_PATH=ROSE/proposed
PROP_INC_PATHS=-I$(abs_top_srcdir)/$(PROP_REL_PATH)/include
PROP_LIB=$(abs_top_builddir)/$(PROP_REL_PATH)/lib/$(PROP_ORG_PATH)/libROSE-Proposed.la

$(PROP_LIB):
	$(MAKE) -C $(abs_top_builddir)/$(PROP_REL_PATH)/lib/$(PROP_ORG_PATH) libROSE-Proposed.la

bin_PROGRAMS=genInstFlowGraph inspectAutoVar
# mfbEnv sage2json

genInstFlowGraph_SOURCES=gen-inst-flow-graph.cpp
genInstFlowGraph_CXXFLAGS=-g $(MFB_INC_PATHS) $(RAT_INC_PATHS) $(ROSE_INCLUDES)
#genInstFlowGraph_LIBADD=$(MFB_LIB_PATHS) $(MFB_LIBS) $(RAT_LIB_PATH) $(RAT_LIBS)
genInstFlowGraph_LDADD=$(LIBS_WITH_RPATH) $(ROSE_LIBS) $(MFB_LIB_SAGE) $(MFB_LIB_UTILS) $(RAT_LIB)

inspectAutoVar_SOURCES=inspect-autovar.cpp
inspectAutoVar_CXXFLAGS=-g $(RAT_INC_PATHS) $(ROSE_INCLUDES)
inspectAutoVar_LDADD=$(LIBS_WITH_RPATH) $(ROSE_LIBS) $(MFB_LIB_UTILS) $(RAT_LIB)

#mfbEnv_SOURCES=mfb-environment.cpp
#mfbEnv_CXXFLAGS=-g $(MFB_INC_PATHS) $(RAT_INC_PATHS) $(PROP_INC_PATHS) $(ROSE_INCLUDES)
#mfbEnv_LIBADD=$(PROP_LIB_PATH) $(PROP_LIBS) $(MFB_LIB_PATHS) $(MFB_LIBS) $(RAT_LIB_PATH) $(RAT_LIBS)
#mfbEnv_LDADD=$(LIBS_WITH_RPATH) $(ROSE_LIBS)

#sage2json_SOURCES=sage-to-json.cpp
#sage2json_CXXFLAGS=-g $(RAT_INC_PATHS) $(ROSE_INCLUDES)
#sage2json_LIBADD=$(RAT_LIB_PATH) $(RAT_LIBS)
#sage2json_LDADD=$(LIBS_WITH_RPATH) $(ROSE_LIBS)

