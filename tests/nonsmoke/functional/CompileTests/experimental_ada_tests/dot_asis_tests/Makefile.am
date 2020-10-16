include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ADA_MAIN=$(top_builddir)/src/frontend/Experimental_Ada_ROSE_Connection/dot_asis/ada_main

$(ADA_MAIN)/obj/run_asis_tool_2:
	cd $(ADA_MAIN); $(MAKE) run_asis_tool_2

RUN_ASIS_TOOL=$(ADA_MAIN)/obj/run_asis_tool_2
GNAT_HOME=$(shell dirname $(shell dirname $(shell which gprbuild)))

ASIS_TESTS_DIR = $(srcdir)/test_units
ASIS_SPEC_TESTS := $(shell find $(ASIS_TESTS_DIR) -name '*.ads')
ASIS_BODY_TESTS := $(shell find $(ASIS_TESTS_DIR) -name '*.adb')

ASIS_TESTS = $(ASIS_SPEC_TESTS) $(ASIS_BODY_TESTS)

#DOT_FILES = $(addsuffix .dot,$(ASIS_TESTS))
DOT_SPEC_FILES = $(notdir ${ASIS_SPEC_TESTS:.ads=.ads.dot})
DOT_BODY_FILES = $(notdir ${ASIS_BODY_TESTS:.adb=.adb.dot})

DOT_FILES = $(DOT_SPEC_FILES) $(DOT_BODY_FILES)

%.ads.dot: $(ASIS_TESTS_DIR)/%.ads  $(RUN_ASIS_TOOL)
	@echo "Testing spec file:" $<
	$(RUN_ASIS_TOOL) --file=$< --gnat_home=$(GNAT_HOME) --output_dir=$(builddir)

%.adb.dot: $(ASIS_TESTS_DIR)/%.adb  $(RUN_ASIS_TOOL)
	@echo "Testing body file:" $<
	$(RUN_ASIS_TOOL) --file=$< --gnat_home=$(GNAT_HOME) --output_dir=$(builddir)


print:
	@echo "Output Makefile variables:"
	@echo "ASIS_SPEC_TESTS = $(ASIS_SPEC_TESTS)"
	@echo "ASIS_BODY_TESTS = $(ASIS_BODY_TESTS)"
	@echo "DOT_SPEC_FILES = $(DOT_SPEC_FILES)"
	@echo "DOT_BODY_FILES = $(DOT_BODY_FILES)"

check-local: 
	@echo "Tests for ASIS."
	@$(MAKE) $(DOT_FILES)

.PHONY: clean-local
clean-local:
	rm -f obj/ *.dot

