include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = testWave testWave2 

testWave_SOURCES = \
	testWave.C

testWave_LDADD = $(ROSE_LIBS)

testWave2_SOURCES = \
	testWave2.C

testWave2_LDADD = $(ROSE_LIBS)

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

TESTCODE_CURRENTLY_FAILING = 

ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)


PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects  = ${ALL_TESTCODES:.c=.o}
TEST_Objects2 = ${ALL_TESTCODES:.c=.o}

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
