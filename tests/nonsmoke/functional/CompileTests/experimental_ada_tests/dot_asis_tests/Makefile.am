include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ADA_MAIN=$(top_builddir)/src/frontend/Experimental_Ada_ROSE_Connection/dot_asis/ada_main
DOT_OPTPUT=$(top_builddir)/tests/nonsmoke/functional/CompileTests/experimental_ada_tests/dot_asis_tests

$(ADA_MAIN)/obj/run_asis_tool_2:
	cd $(ADA_MAIN); $(MAKE) run_asis_tool_2

RUN_ASIS_TOOL=$(ADA_MAIN)/obj/run_asis_tool_2
GNAT_HOME=$(shell dirname $(shell dirname $(shell which gprbuild)))

include $(srcdir)/../AdaTest.mk
# ASIS and ROSE share same test files in a joint directory
ASIS_TESTS_DIR = $(srcdir)/../tests
# ASIS_SPEC_TESTS := $(shell find $(ASIS_TESTS_DIR) -name '*.ads')

# SIS_SPEC_FAILED_TESTS := \
#	all_modes.ads 
ASIS_SPEC_FAILED_TESTS := $(filter %.ads,$(ASIS_FAILING_TESTS)) 

ASIS_SPEC_PASS_TESTS := $(filter %.ads,$(ASIS_PASSING_TESTS))

ASIS_SPEC_TESTS = $(ASIS_SPEC_PASS_TESTS) $(ASIS_SPEC_FAILED_TESTS)

#ASIS_BODY_TESTS := $(shell find $(ASIS_TESTS_DIR) -name '*.adb')

#ASIS_BODY_FAILED_TESTS := \
#        # pragma.adb   #Not sure what happened to this test, it doesn't seem to exist
ASIS_BODY_FAILED_TESTS := $(filter %.adb,$(ASIS_FAILING_TESTS)) 

ASIS_BODY_PASS_TESTS := $(filter %.adb,$(ASIS_PASSING_TESTS))


ASIS_BODY_TESTS = $(ASIS_BODY_PASS_TESTS) $(ASIS_BODY_FAILED_TESTS) 

ASIS_TESTS = $(ASIS_SPEC_TESTS) $(ASIS_BODY_TESTS)

#DOT_FILES = $(addsuffix .dot,$(ASIS_TESTS))
DOT_SPEC_PASS_FILES = ${ASIS_SPEC_PASS_TESTS:.ads=.ads.dot}
DOT_BODY_PASS_FILES = ${ASIS_BODY_PASS_TESTS:.adb=.adb.dot}

DOT_FILES = $(DOT_SPEC_PASS_FILES) $(DOT_BODY_PASS_FILES)

$(DOT_SPEC_PASS_FILES): %.ads.dot: $(ASIS_TESTS_DIR)/%.ads  $(RUN_ASIS_TOOL)
	@echo "Testing spec file:" $<
	$(RUN_ASIS_TOOL) --file=$< --gnat_home=$(GNAT_HOME) --output_dir=$(builddir) 2> /dev/null | (! grep -i "ASIS Error") || (echo "$< failed $?"; exit 1)

$(DOT_BODY_PASS_FILES): %.adb.dot: $(ASIS_TESTS_DIR)/%.adb  $(RUN_ASIS_TOOL)
	@echo "Testing body file:" $<
	$(RUN_ASIS_TOOL) --file=$< --gnat_home=$(GNAT_HOME) --output_dir=$(builddir) 2> /dev/null | (! grep -i "ASIS Error") || (echo "$< failed $?"; exit 1)

PASSING_DOT_DIFF=$(addsuffix .diff, $(DOT_FILES))
REFERENCE_PATH=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/experimental_ada_tests/tests/reference/parser_adapter/dot_graphs

$(PASSING_DOT_DIFF): %.dot.diff: %.dot
	@echo "Comparing dot file:" $<
	if diff $(@:.dot.diff=.dot) $(REFERENCE_PATH)/$(@:.dot.diff=.dot) > $@ ; then echo "$< passed" ; else echo "$< differ; test falied"; rm -rf $@; exit 1; fi

print:
	@echo "Output Makefile variables:"
	@echo "ASIS_SPEC_TESTS = $(ASIS_SPEC_TESTS)"
	@echo "ASIS_BODY_TESTS = $(ASIS_BODY_TESTS)"
	@echo "DOT_SPEC_PASS_FILES = $(DOT_SPEC_PASS_FILES)"
	@echo "DOT_BODY_PASS_FILES = $(DOT_BODY_PASS_FILES)"

check-test-number:
	@test "$(words $(ASIS_PASSING_TESTS))" = "$(shell ls $(DOT_OPTPUT)/*.dot | wc -l)" \
         || { echo $(words $(ASIS_PASSING_TESTS)) tests but only $(shell ls $(DOT_OPTPUT)/*.dot | wc -l) dot files generated; exit 2; } \
         && { echo Total $(words $(ASIS_PASSING_TESTS)) passing tests and $(words $(ASIS_FAILING_TESTS)) failing tests; }

check-local: 
	@echo "Tests ASIS."
	@$(MAKE) $(DOT_FILES)
	@echo "Compare Dot files."
	@$(MAKE) $(PASSING_DOT_DIFF)
	@$(MAKE) check-test-number

.PHONY: clean-local
clean-local:
	rm -rf obj *.dot *.dot.diff

