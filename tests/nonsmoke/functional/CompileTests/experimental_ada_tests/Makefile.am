include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (8/23/2017): Use command line option to alternatively test the experimental fortran frontend.
# ROSE_FLAGS = -rose:verbose 0 -rose:experimental_csharp_frontend -I$(srcdir)
ROSE_FLAGS = -rose:verbose 0 -I$(srcdir)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

MONO_INCLUDE=@MONO_INCLUDE@
ROSYLN_INCLUDE=@ROSYLN_INCLUDE@

AM_CPPFLAGS = $(ROSE_INCLUDES) $(MONO_INCLUDE) $(ROSYLN_INCLUDE)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS) -lexperimentalRosyln

TEST_CSHARP_Objects = ${CSHARP_TESTCODES:.cs=.o}
TEST_F90_Sources = ${CSHARP_TESTCODES:.cs=.cs.passed}

PASSING_TEST_Objects = $(TEST_CSHARP_Objects)

# Go back and build the translator we are using to test Fortran (in case make check 
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).  This does not appear to work!
../../testTranslator:
	cd ../..; $(MAKE) testTranslator

testTranslator: ../../testTranslator

$(TEST_CSHARP_Objects): ../../testTranslator
if ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION
	$(VALGRIND) ../../../testTranslator $(ROSE_FLAGS) -rose:cs -c $(srcdir)/$(@:.o=.cs)
endif

# Simplest test code.
test1: $(srcdir)/unit_1.adb
	../../testTranslator -rose:verbose 0 -rose:ada $(srcdir)/unit_1.adb

# More complex test code.
test2_spec: $(srcdir)/unit_2.ads
	../../testTranslator -rose:verbose 0 -rose:ada $(srcdir)/unit_2.ads

test2_body: $(srcdir)/unit_2.adb
	../../testTranslator -rose:verbose 0 -rose:ada $(srcdir)/unit_2.adb

test2: test2_spec test2_body

test3: $(srcdir)/unit_3.ads
	../../testTranslator -rose:verbose 0 -rose:ada $(srcdir)/unit_3.ads


clean-local:
	rm -f *.o *.adb *.ads *.ali $(srcdir)/obj/* *.dot

check-local:
	@echo "Tests for experimental Csharp frontend."
if ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_csharp_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental csharp frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif

