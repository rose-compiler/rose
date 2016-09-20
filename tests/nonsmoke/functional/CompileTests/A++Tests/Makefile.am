## Process this file with automake to create Makefile.in

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = test2001_11.C

# Files which don't compile (but used to do so) (this has been isolated as test2001_23.C)
# f_op_all.C i_op_all.C d_op_all.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# Restrict to level 6 testing for the moment
# ROSE_FLAGS = -rose:test 6
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

## These tests use the version of A++ source code (really only the header files) 
## that are a part of ROSE (located in ROSE/TESTS/CompileTests/A++Tests
# AM_CPPFLAGS = -I. -I../../.. -I$(top_srcdir)/tests/CompileTests/A++Tests -I$(top_srcdir)/include
# AM_CPPFLAGS = -I. -I../../.. -I$(srcdir)/../A++Code
AM_CPPFLAGS = -I$(srcdir)/../A++Code

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@$(MAKE) $(TEST_Objects)
	@echo "**********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/A++Tests: make check rule complete (terminated normally) ******"
	@echo "**********************************************************************************************"


$(TEST_Objects): $(TESTCODES)
	$(TEST_TRANSLATOR) $(AM_CPPFLAGS) -c $(srcdir)/$(@:.o=.C)

EXTRA_DIST = $(TESTCODES)

OLD_EXTRA_DIST = \
   A++.h array_domain.h data_hash.h dimension_macros.h lazy_expression.h lazy_taskset.h \
   mdi_typedef.h A++_headers.h comm_man.h descriptor.h domain.h inline_func.h lazy_operand.h \
   lazyarray.h optimization.h A++_notemplates.h config.h descriptor_notemplates.h \
   inline_support.h lazy_statement.h machine.h partitioning.h A++_templates.h constants.h \
   descriptor_templates.h index.h lazy_aggregate.h lazy_task.h max_array_dim.h where.h \
   expression_templates.h inline_expression_templates.h $(TESTCODES)

clean-local:
	rm -f *.o rose_test2001_11.C *.sage.C test2001_??.C.show *.ti \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files

