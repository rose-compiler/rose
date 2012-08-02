include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/symbolicAnalysisFramework/common.for.dep.libs

# Saf is the short for symbolic analysis framework.
# it is used to avoid possible name collision 
noinst_LTLIBRARIES = libSafpcfgAnalyses.la
libSafpcfgAnalyses_la_SOURCES = pCFG.C \
				MPIDepAnalysis.C \
				pCFGAnnotations.C \
				pCFGIteratorTransfer.C \
				pCFGIterator.C

include_HEADERS = pCFG.h pCFGIterator.h pCFGIteratorTransfer.h pCFGAnnotations.h MPIDepAnalysis.h

noinst_PROGRAMS = pcfgiterator
pcfgiterator_SOURCES = pCFGIteratorSelfTest.C

LDADD = $(ROSE_LIBS) libSafpcfgAnalyses.la

# Currently dataflow framework cannot handle mpicxx.h
# we use .c to test this code
# more test codes to be added after optimizing the match
TESTCODES_TO_PASS = \
test1.c \
test2.c

EXTRA_DIST = tests

TEST_ANALYSIS = ./pcfgiterator

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict

C_FILES = ${TESTCODES_TO_PASS:.c=.c-o}

$(C_FILES): $(TEST_ANALYSIS) $(srcdir)/tests/$(@:.c-o:.c)
	cp $(srcdir)/tests/$(@:.c-o=.c) $(@:.c-o=.temp.c)
	$(TEST_ANALYSIS) $(ROSE_FLAGS) -I$(srcdir)/tests -c $(@:.c-o=.temp.c) && touch $@

check-c: $(C_FILES)

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

clean-local:
	rm -f       \
		*.temp.c  \
		*.dot     \
		*.c-o     \
		*.html

#check-local: check-c
check-local: check-c_languages check-fortran
	@echo "******************************************************************************************************************************"
	@echo "*** ROSE/projects/symbolicAnalysisFramework/src/parallelCFG/tests : make check rule complete (terminated normally) ***"
	@echo "******************************************************************************************************************************"


#LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) \
#	$(SAF_BUILD_ROOT)/src/parallelCFG/libSafpcfgAnalyses.la
#	$(SAF_BUILD_ROOT)/src/analysis/libSafanalysis.la \
#	$(SAF_BUILD_ROOT)/src/simpleAnalyses/libSafsimpleAnalyses.la \
#	$(SAF_BUILD_ROOT)/src/lattice/libSaflattice.la \
#	$(SAF_BUILD_ROOT)/src/common/libSafcommon.la \
#	$(SAF_BUILD_ROOT)/src/cfgUtils/libSafcfgUtils.la \
#	$(SAF_BUILD_ROOT)/src/variables/libSafvariables.la \
#	$(SAF_BUILD_ROOT)/src/state/libSafstate.la \
#	$(SAF_BUILD_ROOT)/src/rwAccessLabeler/libSafrwAccessLabeler.la \
#	$(SAF_BUILD_ROOT)/src/arrIndexLabeler/libSafarrIndexLabeler.la \
#	$(SAF_BUILD_ROOT)/src/external/libSafexternal.la

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
