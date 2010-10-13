include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafunionFind.la
libSafunionFind_la_SOURCES = unionFind.C
include_HEADERS = unionFind.h

# ----------test program -------------------
#.NOTPARALLEL:selfTest

noinst_PROGRAMS = selfTest
selfTest_SOURCES = selfTest.C
selfTest_LDADD = \
         libSafunionFind.la \
         $(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
         $(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la \
         $(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
         $(SAF_BUILD_ROOT)/src/variables/libSafvariables.la 

selfTest_LDADD += $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local:selfTest
	./selfTest $(srcdir)/test_example.c

EXTRA_DIST =  test_example.c
clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT

