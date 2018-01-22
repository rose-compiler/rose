include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ROSE_FLAGS = -rose:verbose 0 -I$(srcdir)

AM_CPPFLAGS = $(ROSE_INCLUDES) 
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

JOVIAL_TESTCODES = \
 1_2_3_main-program-module_a.jov        \
 1_2_3_program-body_b.jov               \
 1_2_3_program-body_c.jov               \
 5_1_1_integer-primary.jov

JOVIAL_TESTCODES_WORKING_ON = \
 tiny.jov                               \
 1_2_2_non-nested-subroutine_a.jov      \
 1_2_2_non-nested-subroutine_b.jov      \
 1_2_2_procedure-module_a.jov           \
 1_2_2_procedure-module_b.jov           \
 1_2_2_procedure_module_c.jov           \
 1_2_2_procedure_module_d.jov           \
 1_2_2_procedure_module_e.jov           \
 1_2_2_procedure_module_f.jov           \
 1_2_2_procedure_module_g.jov           \
 1_2_3_main-program-module_b.jov        \
 1_2_3_main-program-module_c.jov        \
 1_2_3_main-program-module_d.jov        \
 1_2_3_program-body_a.jov               \
 1_2_3_program-body_d.jov               \
 1_2_3_program-body_e.jov               \
 1_2_3_program-body_f.jov               \
 1_2_3_program-body_g.jov               \
 1_2_3_program-body_h.jov               \
 5_1_1_integer-factor.jov               \
 5_1_1_integer-formula.jov              \
 5_1_1_integer-precedence.jov           \
 5_1_1_integer-term.jov

TEST_JOVIAL_Objects = ${JOVIAL_TESTCODES:.jov=.aterm}
TEST_JOVIAL_Sources = ${JOVIAL_TESTCODES:.jov=.jov.passed}

PASSING_TEST_Objects = $(TEST_JOVIAL_Objects)

# Go back and build the translator we are using to test Jovial (in case make check
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).
../../testParser:
	cd ../..; $(MAKE) testParser

testParser: ../../testParser

$(TEST_JOVIAL_Objects): ../../testParser
if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
	../../testParser $(ROSE_FLAGS) -rose:jovial $(srcdir)/$(@:.aterm=.jov)
endif

clean-local:
	rm -f rose_*.* *.aterm *.dot

check-local:
	@echo "Tests for experimental Jovial frontend."
if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_jovial_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental Jovial frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif
