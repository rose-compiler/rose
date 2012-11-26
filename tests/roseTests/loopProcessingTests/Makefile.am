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
LoopProcessor_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

########################################################################################################################
# Tests.  Just comment out the "TEST_NAMES+=" line to disable a test.  Each test, FOO, will produce either a FOO.passed
# or FOO.failed file with the test output.  Each test runs in a FOO.wrk directory since they would otherwise interfere
# with each other when run in parallel.
########################################################################################################################

# The configuration file. Look here for the actual command that runs for each test. The command can also be found in
# the *.failed files after the "=== CUT ===" line.
EXTRA_DIST += LoopProcessor.conf
LoopProcessor.conf: LoopProcessor

TEST_NAMES += test1
EXTRA_DIST += mm.C mm.ans
test1.passed: LoopProcessor.conf mm.C mm.ans
	@$(RTH_RUN) SWITCHES="-c -bk1 -fs0" INPUT=mm.C ANSWER=mm.ans $< $@

TEST_NAMES += test2
EXTRA_DIST += lufac.C lufac.ans funcs.annot
test2.passed: LoopProcessor.conf lufac.C lufac.ans funcs.annot
	@$(RTH_RUN) SWITCHES="-c -bk1 -fs0 -annot $(srcdir)/funcs.annot" INPUT=lufac.C ANSWER=lufac.ans $< $@

TEST_NAMES += test3
EXTRA_DIST += lufac.C lufac-split.ans funcs.annot
test3.passed: LoopProcessor.conf lufac.C lufac-split.ans funcs.annot
	@$(RTH_RUN) SWITCHES="-c -bk1 -fs0 -splitloop -annot $(srcdir)/funcs.annot" INPUT=lufac.C ANSWER=lufac-split.ans $< $@

TEST_NAMES += test4
EXTRA_DIST += tridvpk.C tridvpk.ans
test4.passed: LoopProcessor.conf tridvpk.C tridvpk.ans
	@$(RTH_RUN) SWITCHES="-c -fs2 -ic1 -opt 1" INPUT=tridvpk.C ANSWER=tridvpk.ans $< $@

TEST_NAMES += test5
EXTRA_DIST += rmatmult3.C rmatmult3.ans
test5.passed: LoopProcessor.conf rmatmult3.C rmatmult3.ans
	@$(RTH_RUN) SWITCHES="-c -bs 60 -fs01" INPUT=rmatmult3.C ANSWER=rmatmult3.ans $< $@

TEST_NAMES += test6
EXTRA_DIST += dgemm_test.C dgemm_test.ans
test6.passed: LoopProcessor.conf dgemm_test.C dgemm_test.ans
	@$(RTH_RUN) SWITCHES="-c -bk1 -unroll -nvar 16" INPUT=dgemm_test.C ANSWER=dgemm_test.ans $< $@

TEST_NAMES += test7
EXTRA_DIST += fusiontest1.C fusiontest1.ans
test7.passed: LoopProcessor.conf fusiontest1.C fusiontest1.ans
	@$(RTH_RUN) SWITCHES="-c -fs2" INPUT=fusiontest1.C ANSWER=fusiontest1.ans $< $@

TEST_NAMES += test8
EXTRA_DIST += mm.C mm-cp0.ans
test8.passed: LoopProcessor.conf mm.C mm-cp0.ans
	@$(RTH_RUN) SWITCHES="-c -cp 0" INPUT=mm.C ANSWER=mm-cp0.ans $< $@

# test9 was commented out in the original "TestDriver" script, but seems to pass anyway.
TEST_NAMES += test9
EXTRA_DIST += lufac.C lufac-cp0.ans funcs.annot
test9.passed: LoopProcessor.conf lufac.C lufac-cp0.ans funcs.annot
	@$(RTH_RUN) SWITCHES="-c -cp 0 -annot $(srcdir)/funcs.annot" INPUT=lufac.C ANSWER=lufac-cp0.ans $< $@

TEST_NAMES += test10
EXTRA_DIST += mm.C mm-cp2-bk3.ans
test10.passed: LoopProcessor.conf mm.C mm-cp2-bk3.ans
	@$(RTH_RUN) SWITCHES="-c -cp 2 -bk3" INPUT=mm.C ANSWER=mm-cp2-bk3.ans $< $@

# test 11 was not present in the original "TestDriver" script


TEST_NAMES += test12
EXTRA_DIST += lufac.C lufac-12.ans
test12.passed: LoopProcessor.conf lufac.C lufac-12.ans
	@$(RTH_RUN) SWITCHES="-c -fs0" INPUT=lufac.C ANSWER=lufac-12.ans $< $@

# test 13 was called "test12" (redundantly) in the original "TestDriver" script
TEST_NAMES += test13
EXTRA_DIST += dgemvT.C dgemvT.ans
test13.passed: LoopProcessor.conf dgemvT.C dgemvT.ans
	@$(RTH_RUN) SWITCHES="-c -fs01 -cp 0" INPUT=dgemvT.C ANSWER=dgemvT.ans $< $@

########################################################################################################################
# Automake targets
########################################################################################################################

check-local: $(addsuffix .passed, $(TEST_NAMES))

clean-local:
	rm -f $(addsuffix .passed, $(TEST_NAMES)) $(addsuffix .failed, $(TEST_NAMES))
	rm -f $(addsuffix .err, $(TEST_NAMES)) $(addsuffix .out, $(TEST_NAMES))
	rm -rf $(addsuffix .wrk, $(TEST_NAMES))
