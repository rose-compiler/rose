#####################################################################################
# Translator-only tests for Rice CAF 2.0 extensions to Fortran                      #
#####################################################################################

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODES_REQUIRED_TO_PASS = \
    abarrier-1.caf \
    acopy-1.caf \
    acopy-2.caf \
    allgather-1.caf \
    allocate-s.caf \
    allocate.caf \
    alltoall-1.caf \
    alltoall-2.caf \
    alltoall-3.caf \
    assign-1.caf \
    assign-2.caf \
    assumed-size-coarray-1.caf \
    assumed-size-coarray-2.caf \
    basic-1.caf \
    broadcast-3.caf \
    broadcast-4.caf \
    broadcast-5.caf \
    broadcast-6.caf \
    caf-allocate.caf \
    caf-derived_type.caf \
    caf-sub.caf \
    caf-team.caf \
    caf-topology.caf \
    caf.caf \
    cmdline-1.caf \
    cmdline-2.caf \
    coarray-3.caf \
    coarray-4.caf \
    coarray-6.caf \
    coarray-allocation-1.caf \
    coarray-bounds-1.caf \
    coarray-bounds-2.caf \
    coarray-semantics-1.caf \
    copointer-declarations.caf \
    copointer-declarations1.caf \
    copointer-declarations2.caf \
    copointer-declarations3.caf \
    copointer-declarations4.caf \
    copointer-declarations5.caf \
    copointer-declarations6.caf \
    copointer-derived-types1.caf \
    copointer-ops.caf \
    coscalar-1.caf \
    cotarget-declarations1.caf \
    derived-type-1.caf \
    derived-type-2.caf \
    derived-type-3.caf \
    derived-type-4.caf \
    derived-type-5.caf \
    derived-type-6.caf \
    derived.caf \
    derivedtype.caf \
    dimensions-1.caf \
    event-1.caf \
    event-2.caf \
    event-3.caf \
    event-4.caf \
    event-5.caf \
    event.caf \
    explicit-shape-1.caf \
    func-decl-1.caf \
    func-decl-4.caf \
    get-1.caf \
    get-2.caf \
    get-3.caf \
    lockset.caf \
    loopbound-1.caf \
    memory-1.caf \
    module-1-mod1.caf \
    module-2-module_table.caf \
    pointer-1.caf \
    put-1.caf \
    put-3.caf \
    remote-read-1.caf \
    remote-write-1.caf \
    team.caf \
    test-copointer3.caf \
    test-copointer4.caf \
    test-copointer4_2.caf \
    test-copointer4_2_1.caf \
    test-copointer4_2_2.caf \
    test-copointer4_3.caf \
    test-copointer5.caf \
    test-copointer5_1.caf \
    test-copointer5_2.caf \
    test-copointer6.caf \
    test-copointer7.caf \
    test-copointer8.caf \
    test-team1.caf \
    test-team2.caf \
    topology.caf \
    withteam.caf

TESTCODE_CURRENTLY_FAILING = \
    X-copointer-derived-types.caf \
    X-copointer-operations.caf \
    X-cotarget-declarations.caf \
    X-test-copointer-codim1.caf \
    X-test-copointer4_1.caf

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) 

ROSE_FLAGS = -rose:caf2.0 -rose:skip_syntax_check -rose:skipfinalCompileStep

PASSING_TEST_Objects = ${TESTCODES:.caf=.o}
TEST_Objects = ${ALL_TESTCODES:.caf=.o}


if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

$(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.caf)
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.caf)

#../../testTranslator:
#	cd ../..; $(MAKE) testTranslator

# put the headers here for now. TODO: Move them to a better place
EXTRA_DIST = $(ALL_TESTCODES) 

check-local:
	@echo "Tests for CAF2 examples."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/CAF2_tests: make check rule complete (terminated normally) *****"
	@echo "***********************************************************************************************"

else

# Fortran support not available in ROSE unless specified on the configure command line.
check-local:
	@echo "***********************************************************************************"
	@echo "****** ROSE/tests/CompileTests/CAF2_tests: FORTRAN NOT CONFIGURED IN ROSE *********"
	@echo "***********************************************************************************"
endif

clean-local:
	rm -f *.o rose_*.* *.rmod

