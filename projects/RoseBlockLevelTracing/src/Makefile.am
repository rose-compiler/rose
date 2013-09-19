include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/input_codes/cxx/Makefile_variables

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects


RoseBlockLevelTracing_includedir=$(prefix)
INCLUDES = $(ROSE_INCLUDES) -I$(RoseBlockLevelTracing_includedir)
LDADD = $(ROSE_LIBS)

bin_PROGRAMS = RoseBlockLevelTracing  RoseTraceReader


RoseBlockLevelTracing_SOURCES=\
	roseBlockLevelTracing.C 

RoseTraceReader_SOURCES=\
	RoseTraceReader.C 



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


### TODO: enable the line below once the bug on inlined class is fixed. I have reported it to Dan in an email on Sep/18/2013. 
### RoseBlockLevelTracing_TEST_TARGETS = $(RoseBlockLevelTracing_TOY_TARGETS)  $(RoseBlockLevelTracingCompileOnly_TEST_TARGETS)
RoseBlockLevelTracing_TEST_TARGETS = $(RoseBlockLevelTracing_TOY_TARGETS)  

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

