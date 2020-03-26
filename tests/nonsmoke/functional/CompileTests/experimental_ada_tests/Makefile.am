include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (8/23/2017): Use command line option to alternatively test the experimental fortran frontend.
# ROSE_FLAGS = -rose:verbose 0 -rose:experimental_csharp_frontend -I$(srcdir)
ROSE_FLAGS = -rose:verbose 0 -rose:skipfinalCompileStep -I$(srcdir)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

# MONO_INCLUDE=@MONO_INCLUDE@
# ROSYLN_INCLUDE=@ROSYLN_INCLUDE@

# AM_CPPFLAGS = $(ROSE_INCLUDES) $(MONO_INCLUDE) $(ROSYLN_INCLUDE)
# AM_LDFLAGS = $(ROSE_RPATHS)
# LDADD    = $(ROSE_LIBS) -lexperimentalRosyln

# TEST_CSHARP_Objects = ${CSHARP_TESTCODES:.cs=.o}
# TEST_F90_Sources = ${CSHARP_TESTCODES:.cs=.cs.passed}

# PASSING_TEST_Objects = $(TEST_CSHARP_Objects)

# Go back and build the translator we are using to test Fortran (in case make check
# is run directly in this directory before make has been run from the root of the
# ROSE directory structure).  This does not appear to work!
../../testTranslator:
	cd ../..; $(MAKE) testTranslator

testTranslator: ../../testTranslator

# $(TEST_CSHARP_Objects): ../../testTranslator
# if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
#	$(VALGRIND) ../../../testTranslator $(ROSE_FLAGS) -rose:cs -c $(srcdir)/$(@:.o=.cs)
# endif

# Simplest test code.
test1: $(srcdir)/unit_1.adb
	../../testTranslator -rose:verbose 0 -rose:ada $(srcdir)/unit_1.adb

# More complex test code.
test2_spec: $(srcdir)/unit_2.ads
	../../testTranslator -rose:verbose 0 -rose:ada $(srcdir)/unit_2.ads

test2_body: $(srcdir)/unit_2.adb
	../../testTranslator -rose:verbose 0 -rose:ada $(srcdir)/unit_2.adb

test2: test2_spec test2_body

test3: $(srcdir)/unit_3.ads
	../../testTranslator -rose:verbose 0 -rose:ada $(srcdir)/unit_3.ads

# TESTCODES_REQUIRED_TO_PASS = minimal.adb empty_package.ads ordinary_type_declaration.ads task_type_declaration.ads variable_declaration.ads \
#   constant_declaration.ads component_declaration.ads procedure_declaration.ads function_declaration.ads parameter_declaration.ads \
#   all_modes.ads procedure_declaration.adb function_declaration.adb package_with_body.adb task_with_body.adb entry_declaration.ads \
#   exception_declaration.ads choice_parameter_specification.adb tagged_definition.ads derived_record.ads simple_expression_range.ads \
#   function_call.adb operator.adb selected_component.ads type_conversion.adb if_statement.adb accept_statement.adb entry_call.adb \
#   loop_statement.adb procedure_call.adb raise_statement.adb

# TESTCODES_REQUIRED_TO_PASS = empty_package.ads
TESTCODES_REQUIRED_TO_PASS_ads = unit_3.ads empty_package.ads ordinary_type_declaration.ads \
   task_type_declaration.ads variable_declaration.ads constant_declaration.ads \
   procedure_declaration.ads function_declaration.ads \
   parameter_declaration.ads entry_declaration.ads exception_declaration.ads \
   tagged_definition.ads derived_record.ads simple_expression_range.ads \
   selected_component.ads nested_package.ads component_declaration.ads all_modes.ads unit_2.ads \
   case_function.ads constants.ads for_loop.adb loop_exit.adb real_typedecl.ads recursive_call.adb \
   short_circuit.adb some_modes.ads     

TESTCODES_REQUIRED_TO_PASS_adb = unit_1.adb minimal.adb procedure_declaration.adb function_declaration.adb \
   package_with_body.adb task_with_body.adb choice_parameter_specification.adb function_call.adb \
   operator.adb type_conversion.adb if_statement.adb accept_statement.adb entry_call.adb \
   loop_statement.adb procedure_call.adb raise_statement.adb unit_2.adb case_function.adb

# Use these files to initialize TESTCODES_REQUIRED_TO_PASS
ORIGINAL_FILE_LIST = minimal.adb empty_package.ads ordinary_type_declaration.ads task_type_declaration.ads variable_declaration.ads \
   constant_declaration.ads component_declaration.ads procedure_declaration.ads function_declaration.ads parameter_declaration.ads \
   all_modes.ads procedure_declaration.adb function_declaration.adb package_with_body.adb task_with_body.adb entry_declaration.ads \
   exception_declaration.ads choice_parameter_specification.adb tagged_definition.ads derived_record.ads simple_expression_range.ads \
   function_call.adb operator.adb selected_component.ads type_conversion.adb if_statement.adb accept_statement.adb entry_call.adb \
   loop_statement.adb procedure_call.adb raise_statement.adb

TESTCODE_CURRENTLY_FAILING_abs =
TESTCODE_CURRENTLY_FAILING_adb =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES_ads = $(TESTCODES_REQUIRED_TO_PASS_ads)
TESTCODES_adb = $(TESTCODES_REQUIRED_TO_PASS_adb)

ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

PASSING_TEST_Objects_ads = ${TESTCODES_ads:.ads=.o}
PASSING_TEST_Objects_adb = ${TESTCODES_adb:.adb=.o}
# PASSING_TEST_Objects_passed = ${TESTCODES:.ads=.o.passed}
# PASSING_TEST_Source_passed = ${TESTCODES:.ads=.c.passed}
# TEST_Objects = ${ALL_TESTCODES:.ads=.o}
# TEST_Objects_passed = ${ALL_TESTCODES:.ads=.o.passed}
# TEST_Source_passed = ${ALL_TESTCODES:.ads=.ads.passed}
# TEST_Exe_Objects = ${TESTCODES_REQUIRED_TO_RUN:.ads=.o}

testTranslator = ../../testTranslator

print:
	@echo "Output Makefile variables:"
	@echo "PASSING_TEST_Objects = $(PASSING_TEST_Objects)"
	@echo "TEST_Source_passed   = $(TEST_Source_passed)"
	@echo "TEST_Objects_passed  = $(TEST_Objects_passed)"

$(PASSING_TEST_Objects_ads):
#	echo "found reference to *.o ads file"
	$(testTranslator) $(ROSE_FLAGS) -rose:ada -c $(srcdir)/$(@:.o=.ads)

$(PASSING_TEST_Objects_adb):
#	echo "found reference to *.o adb file"
	$(testTranslator) $(ROSE_FLAGS) -rose:ada -c $(srcdir)/$(@:.o=.adb)

#$(TEST_Source_passed): %.passed: $(testTranslator) $(top_srcdir)/scripts/test_exit_status
#	$(RTH_RUN) CMD="$(testTranslator) $(ROSE_FLAGS) -rose:ada -c $(srcdir)/$(@:.ads.passed=.ads)" $(top_srcdir)/scripts/test_exit_status $@
#	$(testTranslator) $(ROSE_FLAGS) -rose:ada -c $(srcdir)/$(@:.ads.passed=.ads)

#$(TEST_Exe_Objects): %.o: $(srcdir)/%.c $(testTranslator)
#	$(testTranslator) $(ROSE_FLAGS) -c $<

clean-local:
	rm -f *.o *.adb *.ads *.ali $(srcdir)/obj/* *.dot

check-local:
	@echo "Tests for experimental Csharp frontend."
if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
	@$(MAKE) $(PASSING_TEST_Objects_ads)
	@$(MAKE) $(PASSING_TEST_Objects_adb)
	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_ada_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental Ada frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif

