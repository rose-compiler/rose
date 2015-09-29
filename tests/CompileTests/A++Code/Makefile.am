## Process this file with automake to create Makefile.in
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Comment out these test to make the distcheck tests faster
EXAMPLE_TESTCODES = \
   test_bool.C \
   data_hash.C \
   error_checking.C \
   lazy_statement.C \
   new_operators_notemplates.C \
   descriptor_cache.C \
   lazy_aggregate.C \
   lazy_task.C \
   static_initialization.C \
   comm_man.C \
   descriptor_notemplates.C \
   lazy_taskset.C \
   where.C \
   abstract_op.C \
   index_operator_notemplates.C \
   index.C \
   lazy_operand.C \
   domain.C \
   partitioning.C \
   lazy_expression.C \
   lazyarray.C \
   array_notemplates.C

# DQ (1/18/2012): Failing test codes:
#   lazy_expression.C 
#   lazyarray.C 
#   array_notemplates.C


# Files which don't compile (but used to do so) (this has been isolated as test2001_23.C)
# f_op_all.C i_op_all.C d_op_all.C

# Configure-time automake conditional
if ROSE_USE_LONG_MAKE_CHECK_RULE
# This is the long test of ROSE (takes a while!)
TESTCODES = $(EXAMPLE_TESTCODES)
else
# Short tests (since these test can take a while!)
TESTCODES = \
   test_bool.C \
   data_hash.C
endif

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
if USING_GNU_COMPILER
#	This fails for the Intel icc or icpc compiler.
	@$(MAKE) $(TEST_Objects)
endif
	@echo "*********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/A++Code: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"

../../testTranslator:
	@$(MAKE) -C ../../. testTranslator
$(TEST_Objects): $(TESTCODES) ../../testTranslator
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) $(AM_CPPFLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)

EXTRA_DIST = $(EXAMPLE_TESTCODES) \
   A++.h array_domain.h data_hash.h dimension_macros.h lazy_expression.h lazy_taskset.h \
   mdi_typedef.h A++_headers.h comm_man.h descriptor.h domain.h inline_func.h lazy_operand.h \
   lazyarray.h optimization.h A++_notemplates.h config.h descriptor_notemplates.h \
   inline_support.h lazy_statement.h machine.h partitioning.h A++_templates.h constants.h \
   descriptor_templates.h index.h lazy_aggregate.h lazy_task.h max_array_dim.h where.h \
   expression_templates.h inline_expression_templates.h $(TESTCODES)

clean-local:
	rm -f *.o rose_*.C *.sage.C test2001_??.C.show *.ti \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files

test:
	$(VALGRIND) $(TEST_TRANSLATOR) -DHAVE_CONFIG_H $(AM_CPPFLAGS) -I$(srcdir) -c $(srcdir)/domain.C
