include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

#INCLUDES = $(ROSE_INCLUDES) \
#           -I$(srcdir)/../common

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafcfgUtils.la
libSafcfgUtils_la_SOURCES = CFGRewrite.C  cfgUtils.C  DataflowCFG.C VirtualCFGIterator.C #CallGraphTraverse.C
# parallelNormalForm.C
include_HEADERS = baseCFGIterator.h  CallGraphTraverse.h  CFGRewrite.h  cfgUtils.h  DataflowCFG.h  VirtualCFGIterator.h

# ----------test program -------------------
#  DataflowCFG.cfgToDot.h is missing
#.NOTPARALLEL:selfTest

noinst_PROGRAMS = selfTest
selfTest_SOURCES = selfTest.C
selfTest_LDADD = libSafcfgUtils.la \
                 $(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
		 $(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
		 $(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la

selfTest_LDADD += $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#check-local:selfTest
#	./selfTest $(srcdir)/test_example.cpp
EXTRA_DIST =  test_example.cpp
clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT

