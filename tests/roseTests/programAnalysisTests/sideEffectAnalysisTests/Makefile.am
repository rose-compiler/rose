include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# The testing of the side effect analysis depends upon the side effect analysis, 
# which depends upon the database support within ROSE, which depends upon the use 
# of MySQL, which is an external (and optional) piece of the ROSE puzzle.
if ROSE_USE_DATABASE

INCLUDES = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
# CXX_TEMPLATE_REPOSITORY_PATH = .

bin_PROGRAMS = testSideEffect
testSideEffect_SOURCES = testSideEffect.C
# testSideEffect_DEPENDENCIES = TestDriver.o
testSideEffect_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = testClasses1.C  testClasses2.C  testClasses3.C  testGlobal1.C  testGlobal2.C \
             testLoop.C  testPtr1.C  testPtr2.C  testRef.C  testShadow.C

TESTCODES = 

# to deactivate tests comment out check-local
alt-check-local:
	./testSideEffect $(srcdir)/testClasses1.C
	./testSideEffect $(srcdir)/testClasses2.C
	./testSideEffect $(srcdir)/testClasses3.C
	./testSideEffect $(srcdir)/testGlobal1.C
	./testSideEffect $(srcdir)/testGlobal2.C
	./testSideEffect $(srcdir)/testLoop.C
	./testSideEffect $(srcdir)/testPtr1.C
	./testSideEffect $(srcdir)/testPtr2.C
	./testSideEffect $(srcdir)/testRef.C
	./testSideEffect $(srcdir)/testShadow.C

# DQ (8/26/2004):
# This test currently fails because the data base is not setup before 
# the test is run.  We should likely use the same technique for setting
# up the database as in the test code used in the database test directory.
# This can be fixed later after discussion with Brian White.
failed-check-local:	testSideEffect
	./testSideEffect $(srcdir)/testGlobal1.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testGlobal1.out
	./testSideEffect $(srcdir)/testGlobal2.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testGlobal2.out
	./testSideEffect $(srcdir)/testRef.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testRef.out
	./testSideEffect $(srcdir)/testPtr1.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testPtr1.out
	./testSideEffect $(srcdir)/testClasses1.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testClasses1.out
	./testSideEffect $(srcdir)/testClasses2.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testClasses2.out
	./testSideEffect $(srcdir)/testClasses3.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testClasses3.out
	./testSideEffect $(srcdir)/testLoop.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testLoop.out
	@echo "Side-Effect Tests Passed"

# At least this make check rule will output a message exmplaining that there 
# is a problem. So that we will not easily forget to fix this.
check-local:	testSideEffect
	@echo "*********************************************************************************************************************"
	@echo "Side Effect Tests NOT run, see note in ROSE/tests/roseTests/programAnalysisTests/sideEffectAnalysisTests/Makefile.am!"
	@echo "*********************************************************************************************************************"

# endif for section of Makefile.am which is dependent upon the optional database support
endif

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf rose_mm.C

distclean-local:
	rm -rf Templates.DB






