include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ: This should be handled differently.  No analysis should not have its own makefile include file.
# include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# SAF_SRC_ROOT   = $(top_srcdir)/projects/symbolicAnalysisFramework
SAF_SRC_ROOT   = $(top_srcdir)/src/midend/programAnalysis/genericDataflow/

AM_CPPFLAGS = $(ROSE_INCLUDES) \
           -I$(srcdir) \
           -I$(SAF_SRC_ROOT) \
           -I$(SAF_SRC_ROOT)/analysis \
           -I$(SAF_SRC_ROOT)/arrIndexLabeler \
           -I$(SAF_SRC_ROOT)/cfgUtils \
           -I$(SAF_SRC_ROOT)/lattice \
           -I$(SAF_SRC_ROOT)/rwAccessLabeler \
           -I$(SAF_SRC_ROOT)/simpleAnalyses \
           -I$(SAF_SRC_ROOT)/state \
           -I$(SAF_SRC_ROOT)/variables

noinst_LTLIBRARIES = libSaftaintAnalyses.la
libSaftaintAnalyses_la_SOURCES = taintAnalysis.C \
				 constantPropagation.C

noinst_PROGRAMS = taintAnalysisTest

taintAnalysisTest_SOURCES = taintAnalysisSelfTest.C

include_HEADERS = taintAnalysis.h constantPropagation.h

LDADD = $(ROSE_LIBS) libSaftaintAnalyses.la

TESTCODES_TO_PASS = \
test1.C \
test2.C \
test7.C \
test8.C

# codes with references and pointers
# no support in the framework yet
TESTCODES_FAIL = \
test4.C \
test5.C \
test6.C

TEST_ANALYSIS = ./taintAnalysisTest

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict

EXTRA_DIST = taintFlowTests

#C_FILES = $(TESTCODES_TO_PASS)

C_FILES = ${TESTCODES_TO_PASS:.C=.C-o}
#cp $(srcdir)/taintFlowTests/$(@:.C-o=.C) ./taintFlowTests/$(@:.C-o=.temp.C)

#NOTE: taintFlowTests in the file's path is critical to enable the test

$(C_FILES): $(TEST_ANALYSIS) $(srcdir)/taintFlowTests/$(@:.C-o:.C)
	$(TEST_ANALYSIS) $(ROSE_FLAGS) -I$(srcdir)/taintFlowTests -c $(srcdir)/taintFlowTests/$(@:.C-o=.C)

#$(C_FILES): $(TEST_ANALYSIS)
#	$(TEST_ANALYSIS) $(ROSE_FLAGS) -I$(srcdir)/taintFlowTests -c $(srcdir)/taintFlowTests/$@

check-c: $(C_FILES)

clean-local:
	rm -f *.o rose_*.C
	rm -f *.dot
	rm -f *.mod *.rmod
	rm -f *.html
	rm -rf dbg_imgs
	rm -f *.temp.c
	rm -f *.c-o

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
check-c_languages: check-c
else
check-c_languages: 
endif

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
check-fortran:
else
check-fortran:
endif

distclean-local:

check-local: check-c_languages check-fortran
	@echo "******************************************************************************************************************************"
	@echo "*** ROSE/projects/symbolicAnalysisFramework/src/taintAnalysis/taintFlowTests : make check rule complete (terminated normally) ***"
	@echo "******************************************************************************************************************************"
