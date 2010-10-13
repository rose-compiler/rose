include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafvariables.la
libSafvariables_la_SOURCES = variables.C varSets.C
include_HEADERS = variables.h varSets.h

# ----------test program -------------------
#.NOTPARALLEL:selfTest

noinst_PROGRAMS = selfTest
selfTest_SOURCES = selfTest.C
selfTest_LDADD = \
         $(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
         $(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
         $(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
         $(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la 

selfTest_LDADD += $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Liao 10/7/2010, this does not yet pass
check-local:selfTest
#	./selfTest $(srcdir)/test_example.cpp

EXTRA_DIST =  test_example.cpp
clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT

