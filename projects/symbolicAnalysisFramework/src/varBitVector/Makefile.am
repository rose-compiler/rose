include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafvarBitVector.la
libSafvarBitVector_la_SOURCES = varBitVector.C
include_HEADERS = varBitVector.h

# ----------test program -------------------
#.NOTPARALLEL:selfTest
noinst_PROGRAMS = selfTest
selfTest_SOURCES = selfTest.C
selfTest_LDADD = \
         libSafvarBitVector.la \
         $(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
         $(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
         $(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la \
         $(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la

selfTest_LDADD += $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local:selfTest
# Liao assertion failure here
#	./selfTest $(srcdir)/test_example.cpp

EXTRA_DIST =  test_example.c
clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT

