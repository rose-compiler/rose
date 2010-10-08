include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafompAnal.la
libSafompAnal_la_SOURCES = ompUtils.C #ompNormalForm.C
# Liao 10/7/2010, boost issue with ompNormalForm.C

include_HEADERS = ompUtils.h #ompNormalForm.h
 
# ----------test program -------------------
#.NOTPARALLEL:selfTest errorExample
#noinst_PROGRAMS = selfTest errorExample
#selfTest_SOURCES = selfTest.C
#errorExample_SOURCES = errorExample.C
#selfTest_LDADD = \
#         libSafompAnal.la \
#         $(SAF_BUILD_ROOT)/src/common/libSafcommon.la \
#         $(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
#         $(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
#         $(SAF_BUILD_ROOT)/src/unionFind/libSafunionFind.la \
#         $(SAF_BUILD_ROOT)/src/varBitVector/libSafvarBitVector.la \
#         $(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
#         $(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la
#
#selfTest_LDADD += $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#errorExample_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#check-local:selfTest
#	./selfTest $(srcdir)/test_example.c

EXTRA_DIST =  fanExchange.C fanoutBcast.C fanInGather.C pCFG_Partitioned.C
clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT

