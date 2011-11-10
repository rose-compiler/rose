
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

MINT=@MINT@

TESTCODES=\
	3D-variable-heat.c \
	heat3D.c \
	poisson19.c

TEST_Objects=\
	${TESTCODES:.c=.o}

$(TEST_Objects): $(MINT)
	$(MINT) $@

check-local:
	@$(MAKE) $(TEST_Objects)
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/mint/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

