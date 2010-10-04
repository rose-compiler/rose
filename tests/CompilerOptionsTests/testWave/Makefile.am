include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = testWave testWave2 testWaveTokens testRewrapping 

testRewrapping_SOURCES = \
          testRewrapping.C	
testRewrapping_LDADD = $(ROSE_LIBS)

testWave_SOURCES = \
	testWave.C

testWave_LDADD = $(ROSE_LIBS)

testWave2_SOURCES = \
	testWave2.C

testWave2_LDADD = $(ROSE_LIBS)

testWaveTokens_SOURCES = \
	testWaveTokens.C

testWaveTokens_LDADD = $(ROSE_LIBS)

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
     test10.c \
     test11.C

# DQ (2/4/2010): This test code needs to be checked into the ROSE git repository.
#     testR1.C


TESTCODE_CURRENTLY_FAILING = 

ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)


PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects  = ${ALL_TESTCODES:.c=.o}
TEST_Objects2 = ${ALL_TESTCODES:.c=.o}
TEST_ObjectsTokens = ${ALL_TESTCODES:.c=.o}

$(TEST_Objects): testWave $(srcdir)/$(@:.o=.C)
	rm -f $(@:.o=.c).out
	./testWave -c $(srcdir)/out/$(@:.o=.c) -o $(@:.c=.o)
	if [ "`diff $(srcdir)/out/$(@:.o=.c).out $(@:.o=.c).out | wc -l`" != "0" ]; then exit 1; fi
	if [ "`diff $(srcdir)/out/rose_$(@:.o=.c) rose_$(@:.o=.c) | wc -l`" != "0" ]; then exit 1; fi
	rm -f $(@:.c=.o)

$(TEST_Objects2): testWave2 $(srcdir)/$(@:.o=.C)
	rm -f $(@:.o=.c).out  
	./testWave2 -c $(srcdir)/out/$(@:.o=.c) -o $(@:.c=.o)  -DHYPRE_SEQUENTIAL -DHYPRE_TIMING -DHYPRE_USING_OPENMP
	if [ "`diff $(srcdir)/out/$(@:.o=.c).test2.out $(@:.o=.c).test2.out | wc -l`" != "0" ]; then exit 1; fi
	rm -f $(@:.c=.o)

$(TEST_ObjectsTokens): testWaveTokens $(srcdir)/$(@:.o=.C)
	rm -f tokens_$(@:.o=.c)
	./testWaveTokens -c $(srcdir)/out/$(@:.o=.c) -o $(@:.c=.o)

foo:
	echo tokens_input.cxx | python -c ' \
		import sys \
		while sys.stdin: \
			line = sys.stdin.readline() \
			print ":::",line \
		skip'
#	if [ "`diff $(srcdir)/out/$(@:.o=.c).out $(@:.o=.c).out | wc -l`" != "0" ]; then exit 1; fi
#	if [ "`diff $(srcdir)/out/rose_$(@:.o=.c) rose_$(@:.o=.c) | wc -l`" != "0" ]; then exit 1; fi
	rm -f $(@:.c=.o)

$(TEST_Rewrap): testRewrapping $(srcdir)/$(@:.o=.C)
	rm -f $(@:.o=.c).out
	./testWave -c $(srcdir)/out/$(@:.o=.c) -o $(@:.c=.o)
	if [ "`diff $(srcdir)/out/$(@:.o=.c).out $(@:.o=.c).out | wc -l`" != "0" ]; then exit 1; fi
	if [ "`diff $(srcdir)/out/rose_$(@:.o=.c) rose_$(@:.o=.c) | wc -l`" != "0" ]; then exit 1; fi
	rm -f $(@:.c=.o)


check-local:
	@echo "************************************************************************************************"
	@echo "*** ROSE/tests/CompilerOptionsTests/testWave: make check rule start                          ***"
	@echo "************************************************************************************************"
if ROSE_USING_BOOST_VERSION_1_35
#	It is not helpful to have Boost version dependent tests in ROSE.
	@$(MAKE) $(PASSING_TEST_Objects)
endif
	@echo "************************************************************************************************"
	@echo "*** ROSE/tests/CompilerOptionsTests/testWave: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************************"


CLEANFILES = 

clean-local:
	rm -f *.out *.o

EXTRA_DIST = out 
