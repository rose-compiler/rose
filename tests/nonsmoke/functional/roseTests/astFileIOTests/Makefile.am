include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

COMMA := ,
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)

#------------------------------------------------------------------------------------------------------------------------

# DQ (9/19/2020): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

#------------------------------------------------------------------------------------------------------------------------

ROSE_COMPILER = $(top_builddir)/src/rose-compiler
ROSE_NO_BACKEND_FLAGS = -rose:skip_unparse -rose:skipfinalCompileStep

$(top_builddir)/src/rose-compiler:
	$(MAKE) -C $(top_builddir)/src rose-compiler

#------------------------------------------------------------------------------------------------------------------------
# Creates *.binary files which are used as inputs for other tests.  The creation of the *.binary file is itself a test.

include $(srcdir)/Makefile-pass-IO.inc
Cxx_directory = $(abspath $(top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests)

ast_write_targets = $(addsuffix .binary, $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS))
$(ast_write_targets): %.binary: $(Cxx_directory)/% $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Write Serialize AST for $(notdir $<) [$@]" \
		CMD="$(ROSE_COMPILER) -I$(Cxx_directory) -c $(abspath $<) -rose:ast:write $@" \
		$(TEST_EXIT_STATUS) $@.write.passed

check_ast_write: $(ast_write_targets)

#------------------------------------------------------------------------------------------------------------------------

ast_read_single_targets = $(addsuffix .read.passed, $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS))
$(ast_read_single_targets): %.read.passed: %.binary $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Read Serialize AST for $< [$@]" \
		CMD="$(ROSE_COMPILER) -rose:ast:read $<" \
		$(TEST_EXIT_STATUS) $@.read.passed

check_ast_read_single: $(ast_read_single_targets)

#------------------------------------------------------------------------------------------------------------------------

test_read_multi_00_specimens = test2003_01.C test2003_03.C test2003_05.C
test_read_multi_00_binaries = $(addsuffix .binary, $(test_read_multi_00_specimens))
test_read_multi_00.passed: $(test_read_multi_00_binaries) $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Read Serialized ASTs for $(test_read_multi_00_specimens)" \
		CMD="$(ROSE_COMPILER) \
			-rose:ast:read $(subst $(SPACE),$(COMMA),$(test_read_multi_00_binaries)) \
			-rose:ast:write test_read_multi_00.binary \
			$(ROSE_NO_BACKEND_FLAGS)" \
		$(TEST_EXIT_STATUS) test_read_multi_00.passed

test_read_multi_01_specimens = test2003_10.C test2003_12.C test2003_14.C
test_read_multi_01_binaries = $(addsuffix .binary, $(test_read_multi_01_specimens))
test_read_multi_01.passed: $(test_read_multi_01_binaries) $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Read Serialized ASTs for $(test_read_multi_01_specimens)" \
		CMD="$(ROSE_COMPILER) \
			-rose:ast:read $(subst $(SPACE),$(COMMA),$(test_read_multi_01_binaries)) \
			-rose:ast:write test_read_multi_01.binary \
			$(ROSE_NO_BACKEND_FLAGS)" \
		$(TEST_EXIT_STATUS) test_read_multi_01.passed

test_read_multi_02_specimens = test2003_18.C test2003_20.C test2003_23.C
test_read_multi_02_binaries = $(addsuffix .binary, $(test_read_multi_02_specimens))
test_read_multi_02.passed: $(test_read_multi_02_binaries) $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Read Serialized ASTs for $(test_read_multi_02_specimens)" \
		CMD="$(ROSE_COMPILER) \
			-rose:ast:read $(subst $(SPACE),$(COMMA),$(test_read_multi_02_binaries)) \
			-rose:ast:write test_read_multi_02.binary \
			$(ROSE_NO_BACKEND_FLAGS)" \
		$(TEST_EXIT_STATUS) test_read_multi_02.passed

test_read_multi_03_specimens = test2003_25.C test2003_27.C test2003_35.C
test_read_multi_03_binaries = $(addsuffix .binary, $(test_read_multi_03_specimens))
test_read_multi_03.passed: $(test_read_multi_03_binaries) $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Read Serialized ASTs for $(test_read_multi_03_specimens)" \
		CMD="$(ROSE_COMPILER) \
			-rose:ast:read $(subst $(SPACE),$(COMMA),$(test_read_multi_03_binaries)) \
			-rose:ast:write test_read_multi_03.binary \
			$(ROSE_NO_BACKEND_FLAGS)" \
		$(TEST_EXIT_STATUS) test_read_multi_03.passed

test_read_multi_04_specimens = test2003_02.C test2003_06.C test2003_09.C
test_read_multi_04_binaries = $(addsuffix .binary, $(test_read_multi_04_specimens))
test_read_multi_04.passed: $(test_read_multi_04_binaries) $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Read Serialized ASTs for $(test_read_multi_04_specimens)" \
		CMD="$(ROSE_COMPILER) \
			-rose:ast:read $(subst $(SPACE),$(COMMA),$(test_read_multi_04_binaries)) \
			-rose:ast:write test_read_multi_04.binary \
			$(ROSE_NO_BACKEND_FLAGS)" \
		$(TEST_EXIT_STATUS) test_read_multi_04.passed

test_read_multi_05_specimens = test2003_11.C test2003_15.C test2003_17.C
test_read_multi_05_binaries = $(addsuffix .binary, $(test_read_multi_05_specimens))
test_read_multi_05.passed: $(test_read_multi_05_binaries) $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Read Serialized ASTs for $(test_read_multi_05_specimens)" \
		CMD="$(ROSE_COMPILER) \
			-rose:ast:read $(subst $(SPACE),$(COMMA),$(test_read_multi_05_binaries)) \
			-rose:ast:write test_read_multi_05.binary \
			$(ROSE_NO_BACKEND_FLAGS)" \
		$(TEST_EXIT_STATUS) test_read_multi_05.passed

test_read_multi_06_specimens = test2003_21.C test2003_24.C test2003_26.C
test_read_multi_06_binaries = $(addsuffix .binary, $(test_read_multi_06_specimens))
test_read_multi_06.passed: $(test_read_multi_06_binaries) $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Read Serialized ASTs for $(test_read_multi_06_specimens)" \
		CMD="$(ROSE_COMPILER) \
			-rose:ast:read $(subst $(SPACE),$(COMMA),$(test_read_multi_06_binaries)) \
			-rose:ast:write test_read_multi_06.binary \
			$(ROSE_NO_BACKEND_FLAGS)" \
		$(TEST_EXIT_STATUS) test_read_multi_06.passed

test_read_multi_07_specimens = test2003_28.C test2003_30.C test2003_32.C test2003_36.C
test_read_multi_07_binaries = $(addsuffix .binary, $(test_read_multi_07_specimens))
test_read_multi_07.passed: $(test_read_multi_07_binaries) $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Read Serialized ASTs for $(test_read_multi_07_specimens)" \
		CMD="$(ROSE_COMPILER) \
			-rose:ast:read $(subst $(SPACE),$(COMMA),$(test_read_multi_07_binaries)) \
			-rose:ast:write test_read_multi_07.binary \
			$(ROSE_NO_BACKEND_FLAGS)" \
		$(TEST_EXIT_STATUS) test_read_multi_07.passed

TEST_READ_MULTI_TARGETS= \
	test_read_multi_00.passed \
	test_read_multi_01.passed \
	test_read_multi_02.passed \
	test_read_multi_03.passed \
	test_read_multi_04.passed \
	test_read_multi_05.passed \
	test_read_multi_06.passed \
	test_read_multi_07.passed

check_ast_read_multi: $(TEST_READ_MULTI_TARGETS)

#------------------------------------------------------------------------------------------------------------------------

TEST_READ_MERGE_INPUTS= \
	test_read_multi_00.binary \
	test_read_multi_01.binary \
	test_read_multi_02.binary \
	test_read_multi_03.binary \
	test_read_multi_04.binary \
	test_read_multi_05.binary \
	test_read_multi_06.binary \
	test_read_multi_07.binary

test_read_merged_00.passed: $(TEST_READ_MERGE_INPUTS) $(ROSE_COMPILER)
	@$(RTH_RUN) \
		TITLE="Read Merged ASTs: $(TEST_READ_MERGE_INPUTS)" \
		CMD="$(ROSE_COMPILER) \
			-rose:ast:read $(subst $(SPACE),$(COMMA),$(TEST_READ_MERGE_INPUTS)) \
			-rose:ast:write test_read_merged_00.binary \
			$(ROSE_NO_BACKEND_FLAGS)" \
		$(TEST_EXIT_STATUS) test_read_merged_00.passed

TEST_READ_MERGED_TARGETS=test_read_merged_00.passed

check_ast_read_merged: $(TEST_READ_MERGED_TARGETS)

#------------------------------------------------------------------------------------------------------------------------

check-local: \
		check_ast_write \
		check_ast_read_single
	@echo "*********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/astFileIOTests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************************"

clean-local:
	rm -f *.binary *.passed *.failed rose_* *.o

