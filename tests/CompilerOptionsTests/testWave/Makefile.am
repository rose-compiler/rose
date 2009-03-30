include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = testWave #matchTokenStreamToAST
#bin_PROGRAMS =
testWave_SOURCES = \
	testWave.C

# DQ (3/8/2009): Fixed error: If only SQLite is required, do not include $(QT_LDFLAGS) $(QROSE_LDFLAGS)
# macroRewrapper_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS) $(QROSE_LDFLAGS) -lrt
testWave_LDADD = $(ROSE_LIBS)


# DQ (3/8/2009): Fixed error: -I. and $(BOOST_CPPFLAGS) is included in $(ROSE_INCLUDES).
# INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I.
INCLUDES = $(ROSE_INCLUDES) 

TESTCODES_REQUIRED_TO_PASS = \
     test1.c \
     test2.c \
     test3.c \
     test4.c \
     test5.c \
     test6.c \
     test7.c \
     test8.c \
     test9.c \
     test10.c

TESTCODE_CURRENTLY_FAILING = 

ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)


PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects = ${ALL_TESTCODES:.c=.o}

$(TEST_Objects): testWave $(srcdir)/$(@:.o=.C)
	rm -f $(@:.o=.c).out
	./testWave -c $(srcdir)/out/$(@:.o=.c) -o $(@:.c=.o)
	if [ "`diff $(srcdir)/out/$(@:.o=.c).out $(@:.o=.c).out | wc -l`" != "0" ]; then exit 1; fi
	if [ "`diff $(srcdir)/out/rose_$(@:.o=.c) rose_$(@:.o=.c) | wc -l`" != "0" ]; then exit 1; fi
	rm -f $(@:.c=.o)


test:
	@echo "************************************************************************************************"
	@echo "*** ROSE/tests/CompilerOptionsTests/testWave: make check rule start                          ***"
	@echo "************************************************************************************************"
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "************************************************************************************************"
	@echo "*** ROSE/tests/CompilerOptionsTests/testWave: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************************"


check-local:

CLEANFILES = 

clean-local:
	rm -f *.out *.o

EXTRA_DIST = out 
