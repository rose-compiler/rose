include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_SQLITE_DATABASE

bin_PROGRAMS = macroRewrapper testLinearization testMapping testPruning #matchTokenStreamToAST
#bin_PROGRAMS =
macroRewrapper_SOURCES = \
	waveHelpers.C access-macros.cpp macroRewrapper.C argumentFilterer.C main.C helpFunctions.C

# DQ (3/8/2009): Fixed error: If only SQLite is required, do not include $(QT_LDFLAGS) $(QROSE_LDFLAGS)
# macroRewrapper_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt
macroRewrapper_LDADD = $(ROSE_LIBS)

testLinearization_SOURCES = \
	waveHelpers.C access-macros.cpp macroRewrapper.C argumentFilterer.C testLinearization.C helpFunctions.C

# DQ (3/8/2009): Fixed error: If only SQLite is required, do not include $(QT_LDFLAGS) $(QROSE_LDFLAGS)
# macroRewrapper_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt
testLinearization_LDADD = $(ROSE_LIBS)

testMapping_SOURCES = \
	waveHelpers.C access-macros.cpp macroRewrapper.C argumentFilterer.C testMapping.C helpFunctions.C
testMapping_LDADD = $(ROSE_LIBS)

testPruning_SOURCES = \
	waveHelpers.C access-macros.cpp macroRewrapper.C argumentFilterer.C testPruning.C helpFunctions.C
testPruning_LDADD = $(ROSE_LIBS)


# DQ (3/8/2009): Fixed error: -I. and $(BOOST_CPPFLAGS) is included in $(ROSE_INCLUDES).
# INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.
INCLUDES = $(ROSE_INCLUDES) 

TESTCODES_REQUIRED_TO_PASS = \
     test1.c \
     test2.c \
     test3.c \
     test4.c \
     test5.c \
     test6.c 

TESTCODE_CURRENTLY_FAILING = 

ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

TESTCODES_LINEARIZATION = testLin1.c \
                          testLin2.c


PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects = ${ALL_TESTCODES:.c=.o}


$(TEST_Objects): testPruning $(srcdir)/$(@:.o=.C)
	rm -f $(@:.o=.c).out
	./testPruning -c $(srcdir)/tests/$(@:.o=.c)
	if [ "`diff $(srcdir)/tests/$(@:.o=.c).out $(@:.o=.c).out | wc -l`" != "0" ]; then exit 1; fi


TEST_Linearization = ${TESTCODES_LINEARIZATION:.c=.o}
$(TEST_Linearization): testLinearization $(srcdir)/$(@:.o=.C)
	rm -f $(@:.o=.c).out
	./testLinearization -c $(srcdir)/tests/$(@:.o=.c)
	if [ "`diff $(srcdir)/tests/$(@:.o=.c).out $(@:.o=.c).out | wc -l`" != "0" ]; then exit 1; fi


test:
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/MacroRewrapper: make check rule start                          ***"
	@echo "************************************************************************************"
	@$(MAKE) $(TEST_Linearization)
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "************************************************************************************"
	@echo "*** ROSE/projects/MacroRewrapper: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************"

check-local:

CLEANFILES = 

clean-local:
	rm -rf *.out

endif

EXTRA_DIST = argumentFilterer.h  helpFunctions.h  macro.h  macroRewrapper.h  waveHelpers.h tests

