include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs


# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
#noinst_LTLIBRARIES = libSafvarLatticeVector.la
#libSafvarLatticeVector_la_SOURCES = boolPlus.C VarLatticeVector.C
#include_HEADERS = boolPlus.h  VarLatticeVector.h

## ----------test program -------------------
#.NOTPARALLEL:selfTest
#noinst_PROGRAMS = selfTest
#selfTest_SOURCES = selfTest.C
#selfTest_LDADD = \
#         $(SAF_BUILD_ROOT)/src/varLatticeVector/libSafvarLatticeVector.la \
#         $(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
#         $(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
#         $(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la
#
#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#check-local:selfTest
#	./selfTest test_example.c

EXTRA_DIST =  test_example.c
clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT

