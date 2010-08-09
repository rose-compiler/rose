include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# The testing of the side effect analysis depends upon the side effect analysis,
# which depends upon the database support within ROSE, which depends upon the use
# of sqlite, which is an external (and optional) piece of the ROSE puzzle.

if ROSE_USE_SQLITE_DATABASE

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

check-local:	testSideEffect
	@echo "Run test code ..."
	./testSideEffect $(srcdir)/testGlobal1.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testGlobal1.outx
	-rm *db
	./testSideEffect $(srcdir)/testGlobal2.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testGlobal2.outx
	-rm *db
	./testSideEffect $(srcdir)/testRef.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testRef.outx
	-rm *db
	./testSideEffect $(srcdir)/testPtr1.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testPtr1.outx
	-rm *db
	./testSideEffect $(srcdir)/testClasses1.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testClasses1.outx
	-rm *db
	./testSideEffect $(srcdir)/testClasses2.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testClasses2.outx
	-rm *db
	./testSideEffect $(srcdir)/testClasses3.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testClasses3.outx
	-rm *db
	./testSideEffect $(srcdir)/testLoop.C | grep -i 'side effect' > tmp
	diff tmp $(srcdir)/testLoop.outx
	-rm *db
	@echo "Side-Effect Tests Passed"

# endif for section of Makefile.am which is dependent upon the optional database support
endif

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf rose_mm.C

distclean-local:
	rm -rf Templates.DB
