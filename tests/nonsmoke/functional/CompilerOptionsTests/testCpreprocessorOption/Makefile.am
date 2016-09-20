## Process this file with automake to create Makefile.in
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/tests/CompilerOptionsTests/A++Code -I$(srcdir)
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(srcdir)

# DQ (12/5/2007): A++ does not compile with g++ version 4 and greater so the source
# code has been modified to skip the code when #if ( (__GNUC__ == 3) && (__GNUC_MINOR__ <= 4) )
## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
TESTCODES = data_hash.C 

#     abstract_op.C error_checking.C index_operator_notemplates.C lazy_statement.C \
#     new_operators_notemplates.C descriptor_cache.C lazy_aggregate.C \
#     lazy_task.C static_initialization.C comm_man.C descriptor_notemplates.C \
#     lazy_expression.C lazy_taskset.C index.C lazy_operand.C \
#     lazyarray.C where.C partitioning.C domain.C array_notemplates.C

# codes used to isolate bugs (not very interesting)
# testUnparser.C findNullBlockInEDG.C

# Files which don't compile (but used to do so) (this has been isolated as test2001_23.C)
# f_op_all.C i_op_all.C d_op_all.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# Restrict to level 6 testing for the moment
# ROSE_FLAGS = -rose:test 6

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

# PL (4/15/2014) include syntax uses system header file
# this test has fail because -I has incorrect header file assigned.
# ! at the beginning will negate the FAILED condition.
testSysIncludeOptionOrder: testSysIncludeOptionOrder.c
	! $(CC) -isystem $(srcdir)/doChooseMe -I$(srcdir)/doNotChooseMe -c $(srcdir)/testSysIncludeOptionOrder.c > testSysIncludeOptionOrder.out  
	! $(TEST_TRANSLATOR) -isystem $(srcdir)/doChooseMe -I$(srcdir)/doNotChooseMe -c $(srcdir)/testSysIncludeOptionOrder.c > testSysIncludeOptionOrder.out

# PL (4/14/2014) include syntax uses user header file
# this test has to pass.
testIncludeOptionOrder: testIncludeOptionOrder.c
	$(CC) -isystem $(srcdir)/doNotChooseMe -I$(srcdir)/doChooseMe -c $(srcdir)/testIncludeOptionOrder.c > testIncludeOptionOrder.out
	$(TEST_TRANSLATOR) -isystem $(srcdir)/doNotChooseMe -I$(srcdir)/doChooseMe -c $(srcdir)/testIncludeOptionOrder.c > testIncludeOptionOrder.out


# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@echo "##############################"
	@echo "Testing use of \"-E\" options:"
	@echo "     a standard compiler option which runs the \"C\" preprocessor (cpp) and pipes output to stdout"
	@echo "##############################"
	@$(MAKE) $(TEST_Objects)
	@$(MAKE) testSysIncludeOptionOrder
	@$(MAKE) testIncludeOptionOrder
	@echo "********************************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionTests/testCpreprocessorOption: make check rule complete (terminated normally) ******"
	@echo "********************************************************************************************************************"

# Test the -E option with all of these example files
$(TEST_Objects): $(TESTCODES)
# tps : (01/21/2010) Cannot get this to work. Need Liao to take a look at this
	$(TEST_TRANSLATOR) -E -DHAVE_CONFIG_H -I$(srcdir) -c $(srcdir)/$(@:.o=.C) > $(@:.o=.out.C)
	$(CXX) -DHAVE_CONFIG_H -c $(@:.o=.out.C)

EXTRA_DIST = \
   A++.h array_domain.h data_hash.h dimension_macros.h lazy_expression.h lazy_taskset.h \
   mdi_typedef.h A++_headers.h comm_man.h descriptor.h domain.h inline_func.h lazy_operand.h \
   lazyarray.h optimization.h A++_notemplates.h config.h descriptor_notemplates.h \
   inline_support.h lazy_statement.h machine.h partitioning.h A++_templates.h constants.h \
   descriptor_templates.h index.h lazy_aggregate.h lazy_task.h max_array_dim.h where.h \
   expression_templates.h inline_expression_templates.h $(TESTCODES) doNotChooseMe doChooseMe \
   doNotChooseMe/dummyHeader.h doChooseMe/dummyHeader.h testSysIncludeOptionOrder.c testIncludeOptionOrder.c

clean-local:
	rm -f *.o rose_*.C *.sage.C test2001_??.C.show outputFile *.out.C \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core testSysIncludeOptionOrder.out \
              testIncludeOptionOrder.out rose_testSysIncludeOptionOrder.c rose_testIncludeOptionOrder.c
	rm -rf ti_files






