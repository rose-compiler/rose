
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

MINT=@MINT@

TESTCODES=\
	3D-variable-heat.c \
	heat3D.c \
	poisson19.c

TEST_Results=\
	${TESTCODES:.c=.cu}

$(TEST_Results): $(MINT) $(@:.cu=.c)
	$(MINT) -opt:shared -opt:register -rose:C99 $(srcdir)/$(@:.cu=.c)
clean-local:
	rm -f *.o rose_*.[cC] *.cu
	rm -rf QMTest
EXTRA_DIST = $(TESTCODES)

if !OS_MACOSX
conditional-check-local:
	@$(MAKE) $(TEST_Results)
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/mint/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"
else
conditional-check-local:
endif

check-local: conditional-check-local
