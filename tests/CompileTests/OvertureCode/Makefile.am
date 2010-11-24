## Process this file with automake to create Makefile.in

## Uncomment this line to compile Overture source file: xFDerivative.C
TESTCODES = overtureBug_01.C
## TESTCODES = overtureBug_01.C xFDerivative.C
## TESTCODES = overtureBug_01.C xFDerivative.C defect.C
## TESTCODES = 

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# Restrict to level 6 testing for the moment
# ROSE_FLAGS = -rose:test 6
ROSE_FLAGS =

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

## We don't have to add the A++ header files to the include path since we have placed
## them directly into the test code directory (testing only the compilation with ROSE
## and not the execution!)
## Include the path to A++ and the transformation specification
## INCLUDES = @Axx_INCLUDES@
INCLUDES = -I. -I$(srcdir) -I$(srcdir)/../A++Code

# libTestSource_a_SOURCES = $(TESTCODES)
# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@$(MAKE) $(TEST_Objects)

$(TEST_Objects): $(TESTCODES)
	$(TEST_TRANSLATOR) $(INCLUDES) -c $(srcdir)/$(@:.o=.C)

EXTRA_DIST = $(TESTCODES)

check-local:
	@echo "Tests for Overture examples."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "**************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/OvertureCode: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************************"

clean-local:
	rm -f *.o rose_*.C *.sage.C test2001_??.C.show \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files *.ii























