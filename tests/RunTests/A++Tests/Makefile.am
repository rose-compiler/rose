## Process this file with automake to create Makefile.in

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
if AXXPXX_SPECIFIC_TESTS

# DQ (1/13/2004): Comment out these tests until we have the A++P++Translator updated
# TESTCODES = test1.C
TESTCODES = 

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}
endif

# CXX = ../../src/rose $(ROSE_FLAGS)

## We don't have to add the A++ header files to the include path since we have placed
## them directly into the test code directory (testing only the compilation with ROSE
## and not the execution!)
## Include the path to A++ and the transformation specification
## AM_CPPFLAGS = -I. -I../.. -I$(top_srcdir)/TESTS/CompileTests/A++Tests -I$(top_srcdir)/include -I$(top_srcdir)/ExamplePreprocessors/A++Preprocessor1999/ArrayTransformationGrammar
AM_CPPFLAGS = -I. -I../.. -I$(top_srcdir)/TESTS/CompileTests/A++Tests -I$(top_srcdir)/include 

## libTestSource_a_SOURCES = $(TESTCODES)

## Name the library we will build
## noinst_LIBRARIES = libTestSource.a

## Run each test with this script (required to get the
## LD_LIBRARY_PATH correct when using shared libraries).
## TESTS_ENVIRONMENT = REDIRECT_OUTPUT_TO='$$program.$$nproc.stdout' REDIRECT_ERRORS_TO='$$program.$$nproc.error' sh ../generalized-serial-run

## These tests may or may not have assumed the use
## of some environment variables (we have to see)
## TESTS = $(TESTCODES)

# ROSE_FLAGS = -rose:test 6
# ROSE_FLAGS = -rose:outputGrammarTreeFiles
# ROSE_FLAGS =
ROSE_FLAGS = -DRESTRICT=""

# Use ROSE/ExamplePreprocessors/A++Preprocessor/optimizingArrayPreprocessor instead
PREPROCESSOR = $(top_builddir)/TESTS/testTranslator 

$(TEST_Objects): $(TESTCODES)
	$(PREPROCESSOR) $(ROSE_FLAGS) $(AM_CPPFLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)

# Specify the A++ libraries to be used
LDADD = @Axx_LIBS@

if AXXPXX_SPECIFIC_TESTS
# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "These tests require configuration with path to a fully installed A++P++ Library"
	@$(MAKE) $(TEST_Objects)

else
check-local:
	@echo "\nThese examples require configuration with path to a fully installed A++P++ Library: rerun "
	@echo "     configure --with-AxxPxx=/home/dquinlan/ROSE/A++P++Install"
	@echo "to run these tests. \n\n"

endif

EXTRA_DIST = \
   A++.h array_domain.h data_hash.h dimension_macros.h lazy_expression.h lazy_taskset.h \
   mdi_typedef.h A++_headers.h comm_man.h descriptor.h domain.h inline_func.h lazy_operand.h \
   lazyarray.h optimization.h A++_notemplates.h config.h descriptor_notemplates.h \
   inline_support.h lazy_statement.h machine.h partitioning.h A++_templates.h constants.h \
   descriptor_templates.h index.h lazy_aggregate.h lazy_task.h max_array_dim.h where.h \
   expression_templates.h inline_expression_templates.h \
   test1.C

# DISTCLEANFILES = $(TESTCODES)

clean-local:
	rm -f core *.o rose_*.C *.sage.C *.show test2001_??.C.show \
              *.C.pdf *.C.edg.pdf *.int.C rose_directives_list.txt
	rm -rf ti_files

