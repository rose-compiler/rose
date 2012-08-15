include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# ------------------------------------------------------------------------------
#  Globals
# ------------------------------------------------------------------------------

COMPASS2=$(top_builddir)/projects/compass2/bin/compass2
CHECKER=$(COMPASS2)
COMPASS_PARAMETERS_XSD=$(top_srcdir)/projects/compass2/share/xml/compass_parameters.xsd
TEST_SCRIPT=$(top_srcdir)/projects/compass2/tests/checkers/test_checker.sh

TESTCODES=\
	functionPointersTest3.C \
	functionPointersTest4.C \
	functionPointersTest5.C \
	functionPointersTest6.C \
	functionPointersTest7.C \
	functionPointersTest9.C \
	functionPointersTest10.C \
	functionPointersTest11.C \
	functionPointersTest12.C

# ------------------------------------------------------------------------------
#  Test rules
# ------------------------------------------------------------------------------

.PHONY: $(TESTCODES:.C=.o)
$(TESTCODES:.C=.o): $(TESTCODES)
	$(TEST_SCRIPT) \
		$(CHECKER) \
		$(srcdir)/$(@:.o=.C) \
		"Function pointer detected."

$(COMPASS2):
	$(MAKE) -C $(top_builddir)/projects/compass2

check-local: $(COMPASS2)
	$(MAKE) $(TESTCODES:.C=.o)

# ------------------------------------------------------------------------------
#
# ------------------------------------------------------------------------------

clean-local:
	rm -f \
		rose_*.C \
		*.o \
		*.out \
		*.ti

