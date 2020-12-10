include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = Compass_C_tests Compass_Cxx_tests Compass_OpenMP_tests

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
# TESTCODES = exampleTest_1.C 
TESTCODES = 

# This test just takes a long time and is not critical now that we have seperate C and C++ test directories.
# exampleTest_2.C 

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
# VALGRIND =

# DQ (3/1/2017): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

TEST_Objects = ${TESTCODES:.C=.o}

$(TEST_Objects): ../compassMain ../compass_parameters
	env COMPASS_PARAMETERS=../compass_parameters $(VALGRIND) ../compassMain --compass:silent $(ROSE_FLAGS) -I$(top_builddir) $(AM_CPPFLAGS) -rose:includeFile $(@:.o=) -rose:skip_unparser -rose:skipfinalCompileStep -c $(srcdir)/$(@:.o=.C)

../compass_parameters:
	cd ..; $(MAKE) compass_parameters

../compassMain:
	cd ..; $(MAKE) compassMain

check-local:
	@echo "Tests Compass (and its checkers) using a cross-section of the ROSE C++ test codes..."
# DQ (9/11/2009): Skip tests on Debian system to test ROSE (and focus first on where it works!)
if !OS_VENDOR_DEBIAN
	@$(MAKE) $(TEST_Objects)
endif
	@echo "*****************************************************************************************"
	@echo "****** ROSE/projects/compass/tests: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************"

distclean-local:
	rm -rf Makefile 

clean-local:
	rm -rf *.o rose_*.[cC]

EXTRA_DIST = exampleTest_1.C exampleTest_2.C README
