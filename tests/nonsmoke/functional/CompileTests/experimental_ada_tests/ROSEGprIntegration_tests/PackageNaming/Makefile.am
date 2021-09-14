include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ROSE_COMPILER ?= $(abs_top_builddir)/src/rose-compiler

setup: default.gpr
	cp $< local.gpr
	cp -r $(srcdir)/src .
	echo ${ROSE_COMPILER}
	sed --in-place -re 's|ROSE_CC|'${ROSE_COMPILER}'|g' local.gpr

.PHONY: clean-local
clean-local:
	rm -rf ./obj ./src local.gpr 

check-local:
	@echo "Tests for ROSE gprbuild Ada frontend."
if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
	@$(MAKE) setup
	gprbuild -p -s -P local.gpr
	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_ada_tests/ROSEGprIntegration_tests/PackageNaming: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental Ada frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif
