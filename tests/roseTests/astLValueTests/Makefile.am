include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST =

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS  = testLValues

testLValues_SOURCES = runTest.C

testLValues_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

ROSE_FLAGS = -rose:verbose 0

TEST_TRANSLATOR = ./testLValues $(ROSE_FLAGS)
TESTCODE_INCLUDES =

CLEANFILES = rose_*.C rose_*.c output

check-local: testLValues
	./testLValues $(srcdir)/test_astLValues.C | tee output
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astLValueTests/testLValues: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************************"

clean-local:
	rm -rf test* rose_*.C rose_*.c

distclean-local:
	rm -rf rose_*.C rose_*.c
