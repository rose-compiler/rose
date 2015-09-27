include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = ompAnalyzer
ompAnalyzer_SOURCES = ompAnalyzer.C

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

C_TESTCODES_REQUIRED_TO_PASS = \
	test001_privateLock.c

CXX_TESTCODES_REQUIRED_TO_PASS = 

TESTCODE_CURRENTLY_FAILING = \
  test002_privateReference.cpp


ROSE_FLAGS = -rose:openmp:ast_only --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave
# used to find omp.h, added it as one of  rose headers
TESTCODE_AM_CPPFLAGS = -I$(top_srcdir)/src/frontend/SageIII

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)

PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)

$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c ompAnalyzer
	./ompAnalyzer $(ROSE_FLAGS) $(TESTCODE_AM_CPPFLAGS) -c $<

$(PASSING_CXX_TEST_Objects): %.o: $(srcdir)/%.cpp ompAnalyzer
	./ompAnalyzer ${ROSE_FLAGS} ${TESTCODE_AM_CPPFLAGS} -c $<

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

check-local:
	@echo "Test for ROSE OpenMP Analyzer."
	@$(MAKE) $(PASSING_C_TEST_Objects) 
	@$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/OpenMP_Analysis: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) README 

clean-local:
	rm -f *.o rose_*.* *.out
	rm -rf QMTest


