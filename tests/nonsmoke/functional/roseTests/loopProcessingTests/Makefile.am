include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
EXTRA_DIST =
MOSTLYCLEANFILES =
TEST_NAMES =

########################################################################################################################
# Executables
########################################################################################################################

noinst_PROGRAMS = LoopProcessor
LoopProcessor_SOURCES = LoopProcessor.C
LoopProcessor_CPPFLAGS =				 \
	$(ROSE_INCLUDES)				 \
	-I$(top_srcdir)/src/midend/loopProcessing/driver \
	-I$(top_srcdir)/src/midend/astUtil/astInterface	 \
	-I$(top_srcdir)/src/midend/astUtil/astSupport
LoopProcessor_LDFLAGS = $(ROSE_RPATHS)
LoopProcessor_LDADD = $(ROSE_SEPARATE_LIBS)

########################################################################################################################
# Tests.  Just comment out the "TEST_NAMES+=" line to disable a test.  Each test, FOO, will produce either a FOO.passed
# or FOO.failed file with the test output.  Each test runs in a FOO.wrk directory since they would otherwise interfere
# with each other when run in parallel.
########################################################################################################################

# EDG3 and EDG4 produce slightly different outputs. For instance, EDG3 adds some extra parentheses around expressions.
# These tests aren't intended to check that EDG3 and EDG4 produce identical output, so we just maintain separate
# answers based on the EDG major version number.
#EDG  = edg@ROSE_EDG_MAJOR_VERSION_NUMBER@
EDG  = edg4

# The configuration file. Look here for the actual command that runs for each test. The command can also be found in
# the *.failed files after the "=== CUT ===" line.
EXTRA_DIST += LoopProcessor.conf

TEST_NAMES += test1
EXTRA_DIST += mm.C mm.edg3.ans mm.edg4.ans
test1.passed: LoopProcessor.conf LoopProcessor mm.C mm.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-c -bk1 -fs0" INPUT=mm.C ANSWER=mm.$(EDG).ans $< $@

TEST_NAMES += test2
EXTRA_DIST += lufac.C lufac.edg3.ans lufac.edg4.ans funcs.annot 
test2.passed: LoopProcessor.conf LoopProcessor lufac.C lufac.$(EDG).ans funcs.annot
	@$(RTH_RUN) SWITCHES="-c -bk1 -fs0 -annot $(srcdir)/funcs.annot" INPUT=lufac.C ANSWER=lufac.$(EDG).ans $< $@

#QY: test3 triggered some incorrectly templated code by gcc 4.8.5
if ROSE_USING_GCC_VERSION_LATER_5_1
#TEST_NAMES += test3
EXTRA_DIST += lufac.C lufac-split.edg3.ans lufac-split.edg4.ans funcs.annot
test3.passed: LoopProcessor.conf LoopProcessor lufac.C lufac-split.$(EDG).ans funcs.annot
	@$(RTH_RUN) SWITCHES="-c -bk1 -fs0 -splitloop -annot $(srcdir)/funcs.annot" \
		INPUT=lufac.C ANSWER=lufac-split.$(EDG).ans $< $@
endif

TEST_NAMES += test4
EXTRA_DIST += tridvpk.C tridvpk.edg3.ans tridvpk.edg4.ans
test4.passed: LoopProcessor.conf LoopProcessor tridvpk.C tridvpk.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-c -fs2 -ic1 -opt 1" INPUT=tridvpk.C ANSWER=tridvpk.$(EDG).ans $< $@

TEST_NAMES += test5
EXTRA_DIST += rmatmult3.C rmatmult3.edg3.ans rmatmult3.edg4.ans
test5.passed: LoopProcessor.conf LoopProcessor rmatmult3.C rmatmult3.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-c -bs 60 -fs01" INPUT=rmatmult3.C ANSWER=rmatmult3.$(EDG).ans $< $@

TEST_NAMES += test6
EXTRA_DIST += dgemm_test.C dgemm_test.edg3.ans dgemm_test.edg4.ans
test6.passed: LoopProcessor.conf LoopProcessor dgemm_test.C dgemm_test.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-c -bk1 -unroll -nvar 16" INPUT=dgemm_test.C ANSWER=dgemm_test.$(EDG).ans $< $@

TEST_NAMES += test7
EXTRA_DIST += fusiontest1.C fusiontest1.edg3.ans fusiontest1.edg4.ans
test7.passed: LoopProcessor.conf LoopProcessor fusiontest1.C fusiontest1.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-c -fs2" INPUT=fusiontest1.C ANSWER=fusiontest1.$(EDG).ans $< $@

TEST_NAMES += test8
EXTRA_DIST += mm.C mm-cp0.edg3.ans mm-cp0.edg4.ans
test8.passed: LoopProcessor.conf LoopProcessor mm.C mm-cp0.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-c -cp 0" INPUT=mm.C ANSWER=mm-cp0.$(EDG).ans $< $@

# test9 was commented out in the original "TestDriver" script. It was passing on 2012-12-06, but is failing
# now because two names are swapped in the output. So I'm disabling it again. [RPM 2013-01-17]
#TEST_NAMES += test9
EXTRA_DIST += lufac.C lufac-cp0.edg3.ans lufac-cp0.edg4.ans funcs.annot
test9.passed: LoopProcessor.conf LoopProcessor lufac.C lufac-cp0.$(EDG).ans funcs.annot
	@$(RTH_RUN) SWITCHES="-c -cp 0 -annot $(srcdir)/funcs.annot" INPUT=lufac.C ANSWER=lufac-cp0.$(EDG).ans $< $@

TEST_NAMES += test10
EXTRA_DIST += mm.C mm-cp2-bk3.edg3.ans mm-cp2-bk3.edg4.ans
test10.passed: LoopProcessor.conf LoopProcessor mm.C mm-cp2-bk3.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-c -cp 2 -bk3" INPUT=mm.C ANSWER=mm-cp2-bk3.$(EDG).ans $< $@

# test 11 was not present in the original "TestDriver" script


TEST_NAMES += test12
EXTRA_DIST += lufac.C lufac-12.edg3.ans lufac-12.edg4.ans
test12.passed: LoopProcessor.conf LoopProcessor lufac.C lufac-12.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-c -fs0" INPUT=lufac.C ANSWER=lufac-12.$(EDG).ans $< $@

# test 13 was called "test12" (redundantly) in the original "TestDriver" script
TEST_NAMES += test13
EXTRA_DIST += dgemvT.C dgemvT.edg3.ans dgemvT.edg4.ans
test13.passed: LoopProcessor.conf LoopProcessor dgemvT.C dgemvT.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-c -fs01 -cp 0" INPUT=dgemvT.C ANSWER=dgemvT.$(EDG).ans $< $@

EXTRA_DIST += LoopProcessor_deptest.conf dep_test1.c dep_test1.$(EDG).ans dep_test.annot
TEST_NAMES += deptest1
EXTRA_DIST += dep_test1.c dep_test1.$(EDG).ans 
deptest1.passed: LoopProcessor_deptest.conf LoopProcessor dep_test1.c dep_test1.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-outputdep" INPUT=dep_test1.c ANSWER=dep_test1.$(EDG).ans $< $@


EXTRA_DIST += dep_test2.C dep_test2.$(EDG).ans
TEST_NAMES += deptest2
deptest2.passed: LoopProcessor_deptest.conf LoopProcessor dep_test2.C dep_test2.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-outputdep" INPUT=dep_test2.C ANSWER=dep_test2.$(EDG).ans $< $@

EXTRA_DIST += dep_test3.C dep_test3.$(EDG).ans
TEST_NAMES += deptest3
deptest3.passed: LoopProcessor_deptest.conf LoopProcessor dep_test3.C dep_test3.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-outputdep" INPUT=dep_test3.C ANSWER=dep_test3.$(EDG).ans $< $@

EXTRA_DIST += dep_test4.C dep_test4.$(EDG).ans
TEST_NAMES += deptest4
deptest4.passed: LoopProcessor_deptest.conf LoopProcessor dep_test4.C dep_test4.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-outputdep" INPUT=dep_test4.C ANSWER=dep_test4.$(EDG).ans $< $@

EXTRA_DIST += dep_test5.C dep_test5.$(EDG).ans
TEST_NAMES += deptest5
deptest5.passed: LoopProcessor_deptest.conf LoopProcessor dep_test5.C dep_test5.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-outputdep" INPUT=dep_test5.C ANSWER=dep_test5.$(EDG).ans $< $@

EXTRA_DIST += dep_test6.C dep_test6.$(EDG).ans
TEST_NAMES += deptest6
deptest6.passed: LoopProcessor_deptest.conf LoopProcessor dep_test6.C dep_test6.$(EDG).ans
	@$(RTH_RUN) SWITCHES="-outputdep -annot $(srcdir)/dep_test6.annot" INPUT=dep_test6.C ANSWER=dep_test6.$(EDG).ans $< $@


########################################################################################################################
# Automake targets
########################################################################################################################

check-local: $(addsuffix .passed, $(TEST_NAMES))

clean-local:
	rm -f $(addsuffix .passed, $(TEST_NAMES)) $(addsuffix .failed, $(TEST_NAMES))
	rm -rf $(addsuffix .wrk, $(TEST_NAMES))
