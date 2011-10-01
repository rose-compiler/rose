include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = BT CG EP FT IS LU MG SP

EXTRA_DIST = README.omc  README.org LOG.omc common bin Doc config sys

check-local:
	@echo "Test for ROSE OpenMP Translator."
#	DQ (9/27/2011): This is a redundant rule since there is a local make check rule in each subdirectory.
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
#	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "****************************************************************************************************************"
	@echo "****** ROSE/projects/OpenMP_Translator/tests/npb2.3-omp-c: make check rule complete (terminated normally) ******"
	@echo "****************************************************************************************************************"

