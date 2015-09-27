# DQ (9/3/2006): This is a very old directory of test codes (no longer tested)

## Process this file with automake to create Makefile.in

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES =
#TESTCODES = \
#     test2001_01.C

# Files which don't compile (but used to do so) (this has been isolated as test2001_23.C)
# f_op_all.C i_op_all.C d_op_all.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# ROSE_FLAGS = -rose:outputGrammarTreeFiles
# ROSE_FLAGS = \
#  -rose:outputGrammarTreeFiles -rose:outputGrammarTreeFilesForHeaderFiles \
#  -sage:enable_cp_backend

# Restrict to level 6 testing for the moment
ROSE_FLAGS = -rose:test 6

# -rose:test 3
# -sage:disable_sage_backend

# CXX = ../../../src/rose $(ROSE_FLAGS)

## We don't have to add the A++ header files to the include path since we have placed
## them directly into the test code directory (testing only the compilation with ROSE
## and not the execution!)
## Include the path to A++ and the transformation specification
## AM_CPPFLAGS = @Axx_AM_CPPFLAGS@
MPI_INCLUDE = -I/usr/local/mpi/mpich-1.1.1/include -I/usr/local/mpi/mpich-1.1.1/lib/solaris/ch_p4

AM_CPPFLAGS = -I. -I../../.. -I$(top_srcdir)/tests/CompileTests/P++Tests -I$(top_srcdir)/include $(MPI_INCLUDE)

# libTestSource_a_SOURCES = $(TESTCODES)

## Name the library we will build
# noinst_LIBRARIES = libTestSource.a

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "Tests for P++ (parallel array class) examples."
	@$(MAKE) $(TEST_Objects)
	@echo "**********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/P++Tests: make check rule complete (terminated normally) ******"
	@echo "**********************************************************************************************"

$(TEST_Objects): $(TESTCODES)
	../../testTranslator $(ROSE_FLAGS) $(AM_CPPFLAGS) -c $(srcdir)/$(@:.o=.C)


## Run each test with this script (required to get the
## LD_LIBRARY_PATH correct when using shared libraries).
## TESTS_ENVIRONMENT = REDIRECT_OUTPUT_TO='$$program.$$nproc.stdout' REDIRECT_ERRORS_TO='$$program.$$nproc.error' sh ../generalized-serial-run

## These tests may or may not have assumed the use
## of some environment variables (we have to see)
## TESTS = $(TESTCODES)

EXTRA_DIST = \
     A++.h P++_headers.h conform_array_set.h gs-c.h lazy_aggregate.h machine.h p_domain.h partlist.h send-rcv.h \
     A++_headers.h Parti_config.h conform_enforce.h hash.h lazy_expression.h max_array_dim.h p_inline_func.h \
     pgshash.h List.h constants.h helper.h lazy_operand.h mdi_typedef.h \
     p_inline_support.h pgslib-include-c.h utility-c.h array_domain.h data_hash.h index.h lazy_statement.h \
     mpi-port.h p_lazy_aggregate.h pgslib-names.h utils.h arraylist.h descriptor.h indirect_addressing.h \
     lazy_task.h optimization.h p_lazy_operand.h pgslib-types.h where.h P++.h bsparti.h dimension_macros.h \
     inline_func.h lazy_taskset.h p_data_hash.h p_lazyarray.h port.h P++_config.h comm_man.h domain.h \
     inline_support.h lazyarray.h p_descriptor.h partitioning.h print.h $(TESTCODES)

# A++.h array_domain.h data_hash.h dimension_macros.h lazy_expression.h lazy_taskset.h \
# mdi_typedef.h A++_headers.h comm_man.h descriptor.h domain.h inline_func.h lazy_operand.h \
# lazyarray.h optimization.h config.h descriptor_notemplates.h \
# inline_support.h lazy_statement.h machine.h partitioning.h constants.h \
# descriptor_templates.h index.h lazy_aggregate.h lazy_task.h max_array_dim.h where.h \
# expression_templates.h inline_expression_templates.h $(TESTCODES)

# removed these from list for P++
# A++_notemplates.h
# A++_templates.h 

# DISTCLEANFILES = $(TESTCODES)

clean-local:
	rm -f *.o rose_*.C *.sage.C test2001_??.C.show \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files

