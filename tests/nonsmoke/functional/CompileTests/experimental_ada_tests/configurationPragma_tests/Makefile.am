include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ROSE_COMPILER ?= $(abs_top_builddir)/src/rose-compiler
ROSE_COMPILER ?= $(abs_builddir)/../../../testTranslator

ROSE_OUTPUT_DIR := rose-ada-output
SOURCES:=start.adb 

TEST_TARGETS = $(addprefix $(ROSE_OUTPUT_DIR)/,$(SOURCES))

.PHONY: all
all: passing

.PHONY: passing
passing: $(TEST_TARGETS)

.PHONY: print
print:
	@echo "Output Makefile variables:"
	@echo "PASSING_TEST_Objects = $(PASSING_TEST_Objects)"
	@echo "TEST_Source_passed   = $(TEST_Source_passed)"
	@echo "TEST_Objects_passed  = $(TEST_Objects_passed)"

# copy default.gpr needed by final full-circle test
$(abs_builddir)/gnat.adc: $(srcdir)/tests/gnat.adc
	cp $< $@

.PHONY: clean-local
clean-local:
	rm -rf ./obj ./src *.adc *.dot gnatOutput rose-ada-output 

print-file-summary:
	@echo "Output Makefile variables:"
	@echo "SOURCES = $(SOURCES)"

$(ROSE_OUTPUT_DIR)/%.adb:$(abs_srcdir)/tests/%.adb $(ROSE_COMPILER) $(abs_builddir)/gnat.adc
	$(ROSE_COMPILER) $(ROSE_FLAGS) -rose:skipfinalCompileStep $<

check-local:
	@echo "Tests for ROSE gprbuild Ada frontend."
if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
	$(MAKE) $(PASSING_TEST_Objects_adb)
	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_ada_tests/ROSEGprIntegration_tests/configurationPragma_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental Ada frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif
