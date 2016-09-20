## Process this file with automake to create Makefile.in
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/tests/CompileTests/A++Code -I$(srcdir) 
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/tests/CompilerOptionsTests/A++Code -I$(srcdir) 


## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = test_bool.C data_hash.C \
     abstract_op.C error_checking.C index_operator_notemplates.C lazy_statement.C \
     new_operators_notemplates.C descriptor_cache.C lazy_aggregate.C \
     lazy_task.C static_initialization.C comm_man.C descriptor_notemplates.C \
     lazy_expression.C lazy_taskset.C index.C lazy_operand.C \
     lazyarray.C \
     where.C partitioning.C domain.C array_notemplates.C

# codes used to isolate bugs (not very interesting)
# testUnparser.C findNullBlockInEDG.C

# Files which don't compile (but used to do so) (this has been isolated as test2001_23.C)
# f_op_all.C i_op_all.C d_op_all.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# Restrict to level 6 testing for the moment
ROSE_FLAGS = -rose:test 6

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@$(MAKE) testQuietOutput
	@$(MAKE) $(TEST_Objects)
	@echo "****************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionTests/A++Code: make check rule complete (terminated normally) ******"
	@echo "****************************************************************************************************"


# Test the -E option with all of these example files
$(TEST_Objects): $(TESTCODES)
	@echo "##############################"
	@echo "Testing use of \"-E\" options:"
	@echo "     a standard compiler option which runs the \"C\" preprocessor (cpp) and pipes output to stdout"
	@echo "##############################"
	$(TEST_TRANSLATOR) -DHAVE_CONFIG_H -E $(AM_CPPFLAGS) -c $(srcdir)/$(@:.o=.C) > $(@:.o=.C)
	g++ -DHAVE_CONFIG_H $(AM_CPPFLAGS) -c $(@:.o=.C)

# DQ (8/26/2004): I have selected a file at random to test the make sure that
# the translator built with ROSE has no output!  Output from ROSE translators
# is permited only when SgProject::get_verbose() > 0.
testQuietOutput: test_bool.C 
	@echo "#################################################"
	@echo "Test compilation for any output (should be zero!)"
	@echo "#################################################"
	rm -f outputFile; \
	$(TEST_TRANSLATOR) -DHAVE_CONFIG_H $(AM_CPPFLAGS) -c $(srcdir)/test_bool.C >& outputFile; \
	outputLines=`wc -l < outputFile`; \
	echo "outputLines = $$outputLines"; \
	if test $$outputLines -eq 0; then \
      echo "#############################################################################"; \
      echo "PASSED: Output from translation is quiet (no output detected from translator)"; \
      echo "#############################################################################"; \
   else \
      echo "####################################################################################################"; \
      echo "FAILED: Output from compilation detected (clean up translator to eliminate output when verbose == 0)"; \
      echo "####################################################################################################"; \
      exit 1; \
   fi;

EXTRA_DIST = \
   A++.h array_domain.h data_hash.h dimension_macros.h lazy_expression.h lazy_taskset.h \
   mdi_typedef.h A++_headers.h comm_man.h descriptor.h domain.h inline_func.h lazy_operand.h \
   lazyarray.h optimization.h A++_notemplates.h config.h descriptor_notemplates.h \
   inline_support.h lazy_statement.h machine.h partitioning.h A++_templates.h constants.h \
   descriptor_templates.h index.h lazy_aggregate.h lazy_task.h max_array_dim.h where.h \
   expression_templates.h inline_expression_templates.h $(TESTCODES)

clean-local:
	rm -f *.o *.C rose_*.C *.sage.C test2001_??.C.show outputFile \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files






