include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

C_TESTCODES_REQUIRED_TO_PASS = exemple1.c exemple3.c

TESTCODE_CURRENTLY_FAILING = exemple2.c

ALL_TESTCODES = $(C_TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

C_TEST_Objects = ${C_TESTCODES_REQUIRED_TO_PASS:.c=.o}

../TestRoseToFada:
	$(MAKE) -C ../. TestRoseToFada
	
if ROSE_USE_PPL
../TestRoseToPPL:
	$(MAKE) -C ../. TestRoseToPPL

$(C_TEST_Objects): ../TestRoseToFada ../TestRoseToPPL $(@:.o=.c)
	../TestRoseToFada $(srcdir)/$(@:.o=.c)
	../TestRoseToPPL $(srcdir)/$(@:.o=.c)
else
$(C_TEST_Objects): ../TestRoseToFada $(@:.o=.c)
	../TestRoseToFada $(srcdir)/$(@:.o=.c)
endif

check-local:
	@echo "Test for ROSE to Fada and Parma Polyhedral Library (if enable)."
	@$(MAKE) $(C_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/PolyhedralDependenceAnalysis/tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES)

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cc *.patch

	


