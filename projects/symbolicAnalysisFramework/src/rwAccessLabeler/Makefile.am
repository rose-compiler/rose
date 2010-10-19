include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs


# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafrwAccessLabeler.la
libSafrwAccessLabeler_la_SOURCES = rwAccessLabeler.C
include_HEADERS = rwAccessLabeler.h

# ----------test program -------------------
#SAF_BUILD_ROOT = $(top_builddir)/projects/symbolicAnalysisFramework
#noinst_PROGRAMS = selfTest
#selfTest_SOURCES = selfTest.C
#selfTest_LDADD = $(SAF_BUILD_ROOT)/src/common/libSafcommon.la \
#
#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#check-local:selfTest
#	./selfTest test_example.c
#
#EXTRA_DIST =  test_example.c
