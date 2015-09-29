include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/input_codes/cxx/Makefile_variables

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects


RoseBlockLevelTracing_includedir=$(prefix)
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(RoseBlockLevelTracing_includedir)
LDADD = $(ROSE_LIBS)

bin_PROGRAMS = RoseBlockLevelTracing  RoseTraceReader RoseMultiTraceCommonPathFinder RoseCommonPathToSgNodeMapper


RoseBlockLevelTracing_SOURCES=\
	roseBlockLevelTracing.C 

RoseTraceReader_SOURCES=\
	RoseTraceReader.C 

RoseMultiTraceCommonPathFinder_SOURCES=\
	RoseMultiTraceCommonPathFinder.C

RoseCommonPathToSgNodeMapper_SOURCES=\
	RoseCommonPathToSgNodeMapper.C

RoseBlockLevelTracing_INPUTS = \
	test_simple.C  \
	test_loop.C


RoseBlockLevelTracing_TOY_TARGETS =  $(addsuffix .passed, $(RoseBlockLevelTracing_INPUTS))


$(RoseBlockLevelTracing_TOY_TARGETS): \
	%.passed: ../tests/% RoseBlockLevelTracing RoseTraceReader $(TEST_EXIT_STATUS)
	rm -f `pwd`/rose_`basename $< .C`.rosetrace 
	./RoseBlockLevelTracing -c -rose:projectSpecificDatabaseFile db.txt $<
	$(CXX) -O0 -o rose_`basename $< .C` rose_`basename $<`
	ROSE_TRACE_FULL_FILE_PATH=`pwd`/rose_`basename $< .C`.rosetrace  ./rose_`basename $< .C`
	./RoseTraceReader `pwd`/rose_`basename $< .C`.rosetrace  -rose:projectSpecificDatabaseFile db.txt $<



RoseBlockLevelTracingCompileOnly_SPECIMENS_DIR = $(top_srcdir)/tests/CompileTests/Cxx_tests
RoseBlockLevelTracingCompileOnly_SPECIMENS = $(ROSE__CXX_TESTS)
RoseBlockLevelTracingCompileOnly_TEST_TARGETS = $(addsuffix .passed, $(RoseBlockLevelTracingCompileOnly_SPECIMENS))
RoseBlockLevelTracingCompileOnly_SWITCHES = \
        -rose:projectSpecificDatabaseFile db.txt --edg:no_warnings -w -rose:verbose 0 -I$(RoseBlockLevelTracingCompileOnly_SPECIMENS_DIR) -I$(top_srcdir)/tests/CompileTests/A++Code

$(RoseBlockLevelTracingCompileOnly_TEST_TARGETS): \
	%.passed: $(ROSE__CXX_TESTS_DIR)/% RoseBlockLevelTracing $(TEST_EXIT_STATUS)
	./RoseBlockLevelTracing $(RoseBlockLevelTracingCompileOnly_SWITCHES) -c $< 


RoseBlockLevelTracing_PATH_PREFIX_TARGETS: ../tests/test_common_path.C RoseBlockLevelTracing RoseMultiTraceCommonPathFinder RoseCommonPathToSgNodeMapper
	rm -f test_common_path.*.rosetrace
	./RoseBlockLevelTracing -c -rose:projectSpecificDatabaseFile db.txt $<
	$(CXX) -O0 -o rose_test_common_path rose_test_common_path.C
	ROSE_TRACE_FULL_FILE_PATH=`pwd`/rose_test_common_path.1.rosetrace ./rose_test_common_path 10
	ROSE_TRACE_FULL_FILE_PATH=`pwd`/rose_test_common_path.2.rosetrace ./rose_test_common_path 20
	ROSE_TRACE_FULL_FILE_PATH=`pwd`/rose_test_common_path.3.rosetrace ./rose_test_common_path 30
	./RoseMultiTraceCommonPathFinder `pwd`/rose_test_common_path.1.rosetrace `pwd`/rose_test_common_path.2.rosetrace `pwd`/rose_test_common_path.3.rosetrace -o `pwd`/rose_test_common_path.prefix.rosetrace
	./RoseCommonPathToSgNodeMapper `pwd`/rose_test_common_path.prefix.rosetrace db.txt 
	

### TODO: enable the line below once the bug on inlined class is fixed. I have reported it to Dan in an email on Sep/18/2013. 
### RoseBlockLevelTracing_TEST_TARGETS = $(RoseBlockLevelTracing_TOY_TARGETS)  $(RoseBlockLevelTracingCompileOnly_TEST_TARGETS)
RoseBlockLevelTracing_TEST_TARGETS = $(RoseBlockLevelTracing_TOY_TARGETS)  RoseBlockLevelTracing_PATH_PREFIX_TARGETS

check-local: $(RoseBlockLevelTracing_TEST_TARGETS)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/RoseBlockLevelTracing/: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

clean-local:
	rm -f -- *.out
	rm -f -- *.err
	rm -f -- *.passed
	rm -f -- *.failed
	rm -f -- *.c
	rm -f *.rosetrace
	rm -f *.txt

