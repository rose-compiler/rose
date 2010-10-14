include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafstate.la #libSafstateThreaded.la
libSafstate_la_SOURCES = functionState.C  nodeState.C
include_HEADERS = functionState.h  logical.h  nodeState.h

# need Intel TBB to compile this
# . is allowed or not?
#libSafstateThreaded_la_SOURCES = functionState.C  nodeState.C
#libSafstateThreaded_la_CXXFLAGS = -DTHREADED 

# ----------test program -------------------
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
clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cpp *.patch *.loT

