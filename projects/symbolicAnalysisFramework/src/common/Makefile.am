include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

#INCLUDES = $(ROSE_INCLUDES) \
#           -I$(srcdir)/../common 

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafcommon.la
libSafcommon_la_SOURCES = common.h genUID.C
include_HEADERS = common.h  genUID.h

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
